#ifndef FOOSBALL_TRACKER_APP_MODEL_INCLUDE_PRIVATE_DETECTORS_GOAL_DETECTOR_HPP_
#define FOOSBALL_TRACKER_APP_MODEL_INCLUDE_PRIVATE_DETECTORS_GOAL_DETECTOR_HPP_

#include <cstddef>
#include <cstdint>
#include <opencv2/core/mat.hpp>
#include <opencv2/core/matx.hpp>
#include <opencv2/core/types.hpp>

#include "ball_detector.hpp"
#include "detector.hpp"
#include "detector_types.hpp"
#include "playfield_detector.hpp"

class GoalDetector : public Detector
{
  public:
    enum class GoalSide : std::uint8_t
    {
        kNone,
        kLeft,
        kRight,
    };

    void Detect(const cv::Mat& frame) override;
    void Draw(cv::Mat& frame) const override;

    bool HasGoalScored() const { return goal_scored_; }
    GoalSide GetScoredSide() const { return scored_side_; }
    bool HasLeftGoal() const { return has_left_goal_; }
    bool HasRightGoal() const { return has_right_goal_; }
    const cv::Vec4f& GetLeftGoalLine() const { return left_goal_line_; }
    const cv::Vec4f& GetRightGoalLine() const { return right_goal_line_; }

    void Reset();

  private:
    void DetectGoals(const cv::Mat& frame);
    void DetectGoalScored(const BallDetector::BallMeasurement& ball_measurement);
    bool DidCrossGoalLine(const cv::Point2f& previous_position,
                          const cv::Point2f& current_position,
                          const cv::Vec4f& goal_line) const;
    void ConfirmGoal(const GoalSide scored_side);
    void ClearPendingGoal();
    void ArmPendingGoal(GoalSide side);
    cv::Vec4f BuildGoalLineFromEdge(const cv::Vec4f& edge) const;
    cv::Vec4f BuildGoalLine(const cv::Mat& frame,
                            const Contour& playfield_polygon,
                            std::size_t edge_index) const;
    void DrawGoalLine(cv::Mat& frame, const cv::Vec4f& goal_line) const;
    bool SegmentsIntersect(const cv::Vec4f& segment1, const cv::Vec4f& segment2) const;
    float Cross(const cv::Vec4f& segment, const cv::Point2f& point) const;

    BallDetector ball_detector_{};
    PlayfieldDetector playfield_detector_{};
    cv::Vec4f left_goal_line_;
    cv::Vec4f right_goal_line_;
    bool has_left_goal_{false};
    bool has_right_goal_{false};
    bool waiting_for_goal_confirmation_{false};
    int missed_ball_frames_{0};
    int goal_overlay_frames_remaining_{0};
    GoalSide pending_goal_side_{GoalSide::kNone};
    bool goal_scored_{false};
    GoalSide scored_side_{GoalSide::kNone};
    bool goal_lines_locked_{false};
};

#endif  // FOOSBALL_TRACKER_APP_MODEL_INCLUDE_PRIVATE_DETECTORS_GOAL_DETECTOR_HPP_
