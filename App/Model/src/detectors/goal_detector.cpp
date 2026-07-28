#include "goal_detector.hpp"

#include <bits/std_abs.h>
#include <spdlog/spdlog.h>

#include <algorithm>
#include <cstddef>
#include <limits>
#include <opencv2/core/fast_math.hpp>
#include <opencv2/core/mat.inl.hpp>
#include <opencv2/imgproc.hpp>
#include <string>

#include "detector_types.hpp"

namespace
{
constexpr float kMinSegmentLength{1.0f};
constexpr float kGoalIntersectionEpsilon{1e-3F};
constexpr float kCrossingVelocityThreshold{0.2F};
constexpr int kGoalMissConfirmFrames{10};
constexpr int kGoalOverlayFrames{60};  // 2 seconds at 30 FPS.

bool BuildGoalLineFromEdge(const cv::Point& edge_start,
                           const cv::Point& edge_end,
                           const int frame_height,
                           const cv::Size& frame_size,
                           cv::Point& line_start,
                           cv::Point& line_end)
{
    const cv::Point2f edge_vector{static_cast<float>(edge_end.x - edge_start.x),
                                  static_cast<float>(edge_end.y - edge_start.y)};
    const float edge_length{static_cast<float>(cv::norm(edge_vector))};
    if (edge_length < kMinSegmentLength)
    {
        return false;
    }

    const float requested_half_length{static_cast<float>(std::max(1, frame_height / 4)) / 2.0f};
    const float half_length{std::min(requested_half_length, edge_length / 2.0f)};
    const cv::Point2f unit_vector{edge_vector.x / edge_length, edge_vector.y / edge_length};
    const cv::Point2f center{(static_cast<float>(edge_start.x + edge_end.x) / 2.0f),
                             (static_cast<float>(edge_start.y + edge_end.y) / 2.0f)};

    line_start = cv::Point{cvRound(center.x - (unit_vector.x * half_length)),
                           cvRound(center.y - (unit_vector.y * half_length))};
    line_end = cv::Point{cvRound(center.x + (unit_vector.x * half_length)),
                         cvRound(center.y + (unit_vector.y * half_length))};

    return cv::clipLine(frame_size, line_start, line_end);
}

float Cross(const cv::Point2f& origin, const cv::Point2f& a, const cv::Point2f& b)
{
    const cv::Point2f oa{a - origin};
    const cv::Point2f ob{b - origin};
    return (oa.x * ob.y) - (oa.y * ob.x);
}

bool IsOnSegment(const cv::Point2f& a, const cv::Point2f& b, const cv::Point2f& p)
{
    return p.x >= std::min(a.x, b.x) - kGoalIntersectionEpsilon &&
           p.x <= std::max(a.x, b.x) + kGoalIntersectionEpsilon &&
           p.y >= std::min(a.y, b.y) - kGoalIntersectionEpsilon &&
           p.y <= std::max(a.y, b.y) + kGoalIntersectionEpsilon;
}

bool SegmentsIntersect(const cv::Point2f& p1,
                       const cv::Point2f& p2,
                       const cv::Point2f& q1,
                       const cv::Point2f& q2)
{
    const float d1{Cross(p1, p2, q1)};
    const float d2{Cross(p1, p2, q2)};
    const float d3{Cross(q1, q2, p1)};
    const float d4{Cross(q1, q2, p2)};

    if (((d1 > kGoalIntersectionEpsilon && d2 < -kGoalIntersectionEpsilon) ||
         (d1 < -kGoalIntersectionEpsilon && d2 > kGoalIntersectionEpsilon)) &&
        ((d3 > kGoalIntersectionEpsilon && d4 < -kGoalIntersectionEpsilon) ||
         (d3 < -kGoalIntersectionEpsilon && d4 > kGoalIntersectionEpsilon)))
    {
        return true;
    }

    if (std::abs(d1) <= kGoalIntersectionEpsilon && IsOnSegment(p1, p2, q1))
    {
        return true;
    }

    if (std::abs(d2) <= kGoalIntersectionEpsilon && IsOnSegment(p1, p2, q2))
    {
        return true;
    }

    if (std::abs(d3) <= kGoalIntersectionEpsilon && IsOnSegment(q1, q2, p1))
    {
        return true;
    }

    return std::abs(d4) <= kGoalIntersectionEpsilon && IsOnSegment(q1, q2, p2);
}

float SignedSideOfLine(const cv::Point2f& line_start,
                       const cv::Point2f& line_end,
                       const cv::Point2f& point)
{
    return Cross(line_start, line_end, point);
}
}  // namespace

void GoalDetector::ConfirmGoal(const GoalSide scored_side)
{
    goal_scored_ = true;
    scored_side_ = scored_side;
    goal_overlay_frames_remaining_ = kGoalOverlayFrames;
    waiting_for_goal_confirmation_ = false;
    pending_goal_side_ = GoalSide::kNone;
    missed_ball_frames_ = 0;
}

bool GoalDetector::DidCrossGoalLine(const cv::Point2f& previous_position,
                                    const cv::Point2f& current_position,
                                    const cv::Point& line_start,
                                    const cv::Point& line_end) const
{
    const cv::Point2f line_start_f{static_cast<float>(line_start.x),
                                   static_cast<float>(line_start.y)};
    const cv::Point2f line_end_f{static_cast<float>(line_end.x), static_cast<float>(line_end.y)};

    if (SegmentsIntersect(previous_position, current_position, line_start_f, line_end_f))
    {
        return true;
    }

    const float previous_side{SignedSideOfLine(line_start_f, line_end_f, previous_position)};
    const float current_side{SignedSideOfLine(line_start_f, line_end_f, current_position)};

    if (std::abs(previous_side) <= kGoalIntersectionEpsilon ||
        std::abs(current_side) <= kGoalIntersectionEpsilon)
    {
        return true;
    }

    return (previous_side < -kGoalIntersectionEpsilon && current_side > kGoalIntersectionEpsilon) ||
           (previous_side > kGoalIntersectionEpsilon && current_side < -kGoalIntersectionEpsilon);
}

void GoalDetector::DetectGoals(const cv::Mat& frame)
{
    has_left_goal_ = false;
    has_right_goal_ = false;
    left_goal_line_ = cv::Vec4i{};
    right_goal_line_ = cv::Vec4i{};

    playfield_detector_.Detect(frame);
    if (!playfield_detector_.HasDetection())
    {
        return;
    }

    const Contour& playfield_polygon{playfield_detector_.GetPolygon()};
    if (playfield_polygon.size() < 2)
    {
        return;
    }

    std::size_t left_edge_index{0};
    std::size_t right_edge_index{0};
    int left_edge_midpoint_x{std::numeric_limits<int>::max()};
    int right_edge_midpoint_x{std::numeric_limits<int>::min()};

    for (std::size_t i{0}; i < playfield_polygon.size(); ++i)
    {
        const cv::Point& edge_start{playfield_polygon[i]};
        const cv::Point& edge_end{playfield_polygon[(i + 1) % playfield_polygon.size()]};
        const int midpoint_x{edge_start.x + edge_end.x};

        if (midpoint_x < left_edge_midpoint_x)
        {
            left_edge_midpoint_x = midpoint_x;
            left_edge_index = i;
        }

        if (midpoint_x > right_edge_midpoint_x)
        {
            right_edge_midpoint_x = midpoint_x;
            right_edge_index = i;
        }
    }

    const cv::Point& left_edge_start{playfield_polygon[left_edge_index]};
    const cv::Point& left_edge_end{
      playfield_polygon[(left_edge_index + 1) % playfield_polygon.size()]};
    cv::Point left_goal_start;
    cv::Point left_goal_end;
    has_left_goal_ = BuildGoalLineFromEdge(
      left_edge_start, left_edge_end, frame.rows, frame.size(), left_goal_start, left_goal_end);
    if (has_left_goal_)
    {
        left_goal_line_ =
          cv::Vec4i{left_goal_start.x, left_goal_start.y, left_goal_end.x, left_goal_end.y};
    }

    const cv::Point& right_edge_start{playfield_polygon[right_edge_index]};
    const cv::Point& right_edge_end{
      playfield_polygon[(right_edge_index + 1) % playfield_polygon.size()]};
    cv::Point right_goal_start;
    cv::Point right_goal_end;
    has_right_goal_ = BuildGoalLineFromEdge(
      right_edge_start, right_edge_end, frame.rows, frame.size(), right_goal_start, right_goal_end);
    if (has_right_goal_)
    {
        right_goal_line_ =
          cv::Vec4i{right_goal_start.x, right_goal_start.y, right_goal_end.x, right_goal_end.y};
    }
}

void GoalDetector::DetectGoalScored(const BallDetector::BallMeasurement& ball_measurement)
{
    const auto clear_pending_goal = [&]()
    {
        waiting_for_goal_confirmation_ = false;
        pending_goal_side_ = GoalSide::kNone;
        missed_ball_frames_ = 0;
    };

    const auto arm_pending_goal = [&](const GoalSide side)
    {
        waiting_for_goal_confirmation_ = true;
        pending_goal_side_ = side;
        missed_ball_frames_ = 0;
    };

    if (!has_left_goal_ && !has_right_goal_)
    {
        clear_pending_goal();
        return;
    }

    if (!ball_measurement.found)
    {
        if (waiting_for_goal_confirmation_)
        {
            ++missed_ball_frames_;
            if (missed_ball_frames_ >= kGoalMissConfirmFrames)
            {
                ConfirmGoal(pending_goal_side_);
            }
        }
        return;
    }

    const cv::Point2f current_ball_position{static_cast<float>(ball_measurement.position.x),
                                            static_cast<float>(ball_measurement.position.y)};
    const cv::Point2f current_ball_speed{static_cast<float>(ball_measurement.speed.x),
                                         static_cast<float>(ball_measurement.speed.y)};

    if (waiting_for_goal_confirmation_)
    {
        // Ball was detected before reaching the missing-frame threshold, so cancel pending goal.
        clear_pending_goal();
    }

    if (cv::norm(current_ball_speed) >= kCrossingVelocityThreshold)
    {
        const cv::Point2f projected_ball_position{
          current_ball_position +
          (current_ball_speed * static_cast<float>(detector_types::kVelocityArrowScale))};
        const cv::Point left_goal_start{left_goal_line_[0], left_goal_line_[1]};
        const cv::Point left_goal_end{left_goal_line_[2], left_goal_line_[3]};
        const cv::Point right_goal_start{right_goal_line_[0], right_goal_line_[1]};
        const cv::Point right_goal_end{right_goal_line_[2], right_goal_line_[3]};

        if (has_left_goal_ &&
            DidCrossGoalLine(
              current_ball_position, projected_ball_position, left_goal_start, left_goal_end))
        {
            arm_pending_goal(GoalSide::kLeft);
        }
        else if (has_right_goal_ && DidCrossGoalLine(current_ball_position,
                                                     projected_ball_position,
                                                     right_goal_start,
                                                     right_goal_end))
        {
            arm_pending_goal(GoalSide::kRight);
        }
    }
}

void GoalDetector::Detect(const cv::Mat& frame)
{
    goal_scored_ = false;

    if (goal_overlay_frames_remaining_ > 0)
    {
        --goal_overlay_frames_remaining_;
    }

    if (frame.empty())
    {
        waiting_for_goal_confirmation_ = false;
        pending_goal_side_ = GoalSide::kNone;
        missed_ball_frames_ = 0;
        return;
    }

    DetectGoals(frame);

    ball_detector_.Detect(frame);
    const BallDetector::BallMeasurement& ball_measurement{ball_detector_.GetMeasurement()};
    DetectGoalScored(ball_measurement);
}

void GoalDetector::Draw(cv::Mat& frame) const
{
    if (frame.empty() || (!has_left_goal_ && !has_right_goal_))
    {
        return;
    }

    const cv::Scalar goal_color{0, 255, 255};

    if (has_left_goal_)
    {
        const cv::Point left_goal_start{left_goal_line_[0], left_goal_line_[1]};
        const cv::Point left_goal_end{left_goal_line_[2], left_goal_line_[3]};
        cv::line(frame,
                 left_goal_start,
                 left_goal_end,
                 goal_color,
                 detector_types::kGoalDrawThickness,
                 cv::LINE_AA);
    }

    if (has_right_goal_)
    {
        const cv::Point right_goal_start{right_goal_line_[0], right_goal_line_[1]};
        const cv::Point right_goal_end{right_goal_line_[2], right_goal_line_[3]};
        cv::line(frame,
                 right_goal_start,
                 right_goal_end,
                 goal_color,
                 detector_types::kGoalDrawThickness,
                 cv::LINE_AA);
    }

    if (goal_scored_)
    {
        spdlog::info("Goal scored on: {}", (scored_side_ == GoalSide::kLeft) ? "LEFT" : "RIGHT");
    }

    if (goal_overlay_frames_remaining_ > 0)
    {
        const std::string goal_label =
          (scored_side_ == GoalSide::kLeft) ? "GOAL: LEFT" : "GOAL: RIGHT";
        constexpr double goal_label_scale{2.0};
        constexpr int goal_label_thickness{5};
        int baseline{0};
        const cv::Size label_size = cv::getTextSize(
          goal_label, cv::FONT_HERSHEY_SIMPLEX, goal_label_scale, goal_label_thickness, &baseline);
        const cv::Point label_origin{(frame.cols - label_size.width) / 2,
                                     (frame.rows + label_size.height) / 2};
        const cv::Scalar goal_label_color{180, 105, 255};

        cv::putText(frame,
                    goal_label,
                    label_origin,
                    cv::FONT_HERSHEY_SIMPLEX,
                    goal_label_scale,
                    goal_label_color,
                    goal_label_thickness,
                    cv::LINE_AA);
    }
}