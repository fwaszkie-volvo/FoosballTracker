#include "goal_detector.hpp"

#include <spdlog/spdlog.h>

#include <cstddef>
#include <limits>
#include <opencv2/core/mat.inl.hpp>
#include <opencv2/imgproc.hpp>
#include <string>

#include "detector_types.hpp"

constexpr float kMinSegmentLength{1.0f};
constexpr float kGoalLineLengthRatio{0.8f};
constexpr float kGoalIntersectionEpsilon{1e-3F};
constexpr float kCrossingVelocityThreshold{0.2F};
constexpr int kGoalMissConfirmFrames{10};
constexpr int kGoalOverlayFrames{60};  // 2 seconds at 30 FPS.

float GoalDetector::Cross(const cv::Vec4f& segment, const cv::Point2f& point) const
{
    const cv::Point2f origin{segment[0], segment[1]};
    const cv::Point2f a{segment[2], segment[3]};
    const cv::Point2f oa{a - origin};
    const cv::Point2f ob{point - origin};
    return (oa.x * ob.y) - (oa.y * ob.x);
}

bool GoalDetector::SegmentsIntersect(const cv::Vec4f& segment1, const cv::Vec4f& segment2) const
{
    const cv::Point2f p1{segment1[0], segment1[1]};
    const cv::Point2f p2{segment1[2], segment1[3]};
    const cv::Point2f q1{segment2[0], segment2[1]};
    const cv::Point2f q2{segment2[2], segment2[3]};

    const float d1{Cross(segment1, q1)};
    const float d2{Cross(segment1, q2)};
    const float d3{Cross(segment2, p1)};
    const float d4{Cross(segment2, p2)};

    // Check proper intersection (opposite sides)
    return (((d1 > kGoalIntersectionEpsilon && d2 < -kGoalIntersectionEpsilon) ||
             (d1 < -kGoalIntersectionEpsilon && d2 > kGoalIntersectionEpsilon)) &&
            ((d3 > kGoalIntersectionEpsilon && d4 < -kGoalIntersectionEpsilon) ||
             (d3 < -kGoalIntersectionEpsilon && d4 > kGoalIntersectionEpsilon)));
}

cv::Vec4f GoalDetector::BuildGoalLineFromEdge(const cv::Vec4f& edge) const
{
    const cv::Point2f edge_vector{edge[2] - edge[0], edge[3] - edge[1]};
    const float edge_length{static_cast<float>(cv::norm(edge_vector))};
    if (edge_length < kMinSegmentLength)
    {
        return cv::Vec4f{};
    }

    const float half_new_length{(edge_length * kGoalLineLengthRatio) / 2.0f};
    const cv::Point2f unit_vector{edge_vector.x / edge_length, edge_vector.y / edge_length};
    const cv::Point2f center{(edge[0] + edge[2]) / 2.0f, (edge[1] + edge[3]) / 2.0f};

    const cv::Point2f line_start{center.x - (unit_vector.x * half_new_length),
                                 center.y - (unit_vector.y * half_new_length)};
    const cv::Point2f line_end{center.x + (unit_vector.x * half_new_length),
                               center.y + (unit_vector.y * half_new_length)};

    return cv::Vec4f{line_start.x, line_start.y, line_end.x, line_end.y};
}

void GoalDetector::ClearPendingGoal()
{
    waiting_for_goal_confirmation_ = false;
    pending_goal_side_             = GoalSide::kNone;
    missed_ball_frames_            = 0;
}

void GoalDetector::ArmPendingGoal(GoalSide side)
{
    waiting_for_goal_confirmation_ = true;
    pending_goal_side_             = side;
    missed_ball_frames_            = 0;
}

cv::Vec4f GoalDetector::BuildGoalLine(const cv::Mat& frame,
                                      const Contour& playfield_polygon,
                                      std::size_t edge_index) const
{
    const cv::Point& edge_start{playfield_polygon[edge_index]};
    const cv::Point& edge_end{playfield_polygon[(edge_index + 1) % playfield_polygon.size()]};
    const cv::Vec4f edge{static_cast<float>(edge_start.x),
                         static_cast<float>(edge_start.y),
                         static_cast<float>(edge_end.x),
                         static_cast<float>(edge_end.y)};
    return BuildGoalLineFromEdge(edge);
}

void GoalDetector::DrawGoalLine(cv::Mat& frame, const cv::Vec4f& goal_line) const
{
    const cv::Scalar goal_color{0, 255, 255};
    const cv::Point start{static_cast<int>(goal_line[0]), static_cast<int>(goal_line[1])};
    const cv::Point end{static_cast<int>(goal_line[2]), static_cast<int>(goal_line[3])};
    cv::line(frame, start, end, goal_color, detector_types::kGoalDrawThickness, cv::LINE_AA);
}

void GoalDetector::ConfirmGoal(const GoalSide scored_side)
{
    goal_scored_                   = true;
    scored_side_                   = scored_side;
    goal_overlay_frames_remaining_ = kGoalOverlayFrames;
    ClearPendingGoal();
}

bool GoalDetector::DidCrossGoalLine(const cv::Point2f& previous_position,
                                    const cv::Point2f& current_position,
                                    const cv::Vec4f& goal_line) const
{
    const cv::Vec4f ball_trajectory{
      previous_position.x, previous_position.y, current_position.x, current_position.y};
    return SegmentsIntersect(ball_trajectory, goal_line);
}

void GoalDetector::DetectGoals(const cv::Mat& frame)
{
    has_left_goal_   = false;
    has_right_goal_  = false;
    left_goal_line_  = cv::Vec4f{};
    right_goal_line_ = cv::Vec4f{};

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
            left_edge_index      = i;
        }

        if (midpoint_x > right_edge_midpoint_x)
        {
            right_edge_midpoint_x = midpoint_x;
            right_edge_index      = i;
        }
    }

    left_goal_line_  = BuildGoalLine(frame, playfield_polygon, left_edge_index);
    has_left_goal_   = (left_goal_line_ != cv::Vec4f{});
    right_goal_line_ = BuildGoalLine(frame, playfield_polygon, right_edge_index);
    has_right_goal_  = (right_goal_line_ != cv::Vec4f{});
}

void GoalDetector::DetectGoalScored(const BallDetector::BallMeasurement& ball_measurement)
{
    if (!has_left_goal_ && !has_right_goal_)
    {
        ClearPendingGoal();
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

    const cv::Point2f current_ball_position{ball_measurement.position};
    const cv::Point2f current_ball_speed{ball_measurement.speed};

    if (waiting_for_goal_confirmation_)
    {
        // Ball was detected before reaching the missing-frame threshold, so cancel pending goal.
        ClearPendingGoal();
    }

    if (cv::norm(current_ball_speed) >= kCrossingVelocityThreshold)
    {
        const cv::Point2f projected_ball_position{
          current_ball_position +
          (current_ball_speed * static_cast<float>(detector_types::kVelocityArrowScale))};

        if (has_left_goal_ &&
            DidCrossGoalLine(current_ball_position, projected_ball_position, left_goal_line_))
        {
            ArmPendingGoal(GoalSide::kLeft);
        }
        else if (has_right_goal_ &&
                 DidCrossGoalLine(current_ball_position, projected_ball_position, right_goal_line_))
        {
            ArmPendingGoal(GoalSide::kRight);
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
        pending_goal_side_             = GoalSide::kNone;
        missed_ball_frames_            = 0;
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
        DrawGoalLine(frame, left_goal_line_);
    if (has_right_goal_)
        DrawGoalLine(frame, right_goal_line_);

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