#ifndef FOOSBALL_TRACKER_APP_MODEL_INCLUDE_PRIVATE_DETECTORS_BALL_DETECTOR_HPP_
#define FOOSBALL_TRACKER_APP_MODEL_INCLUDE_PRIVATE_DETECTORS_BALL_DETECTOR_HPP_

#include <deque>
#include <opencv2/core/mat.hpp>
#include <opencv2/core/types.hpp>

#include "detector.hpp"
#include "playfield_detector.hpp"
#include "pressure_index_detector.hpp"

class BallDetector : public Detector
{
  public:
    struct BallMeasurement
    {
        bool found{};
        cv::Point2f position;
        cv::Point2f speed;
        double size{0.0};
    };

    void Detect(const cv::Mat& frame) override;
    void Draw(cv::Mat& frame) const override;

    const BallMeasurement& GetMeasurement() const { return measurement_; }
    const PressureIndexDetector& GetPressureIndex() const { return pressure_index_; }

  private:
    struct DetectionCandidate
    {
        bool found{};
        cv::Point center;
        double score{-1.0};
    };

    bool IsCircleInsideFrame(const cv::Point& center, const int radius, const cv::Size& size) const;
    void ResetBestCandidate();
    void UpdateCandidate(const cv::Point& center, double score);
    cv::Point2f ComputeWindowSpeed() const;
    void UpdateMeasurement(const cv::Point& center);
    cv::Mat BuildColorMask(const cv::Mat& frame) const;
    cv::Mat BuildGrayFrame(const cv::Mat& frame, const cv::Mat& playfield_mask) const;
    cv::Mat BuildDetectionMask(const cv::Mat& color_mask,
                               const cv::Mat& gray,
                               const cv::Mat& playfield_mask,
                               bool& has_reliable_foreground);
    void ScoreHoughCandidates(const cv::Mat& frame, const cv::Mat& gray, const cv::Mat& mask);
    void UpdateTrackingState();
    void ResetTrackingState();
    void CollectHoughCandidate(const cv::Mat& frame,
                               const cv::Mat& mask,
                               const cv::Point& center,
                               const int radius);
    cv::Mat BuildForegroundMask(const cv::Mat& gray, const cv::Mat& playfield_mask);
    double ComputeMetersPerPixel() const;
    double ComputeBallSpeedMetersPerSecond() const;

    PlayfieldDetector playfield_detector_{};
    PressureIndexDetector pressure_index_{};
    DetectionCandidate best_candidate_{};
    BallMeasurement measurement_{};
    cv::Point2f previous_position_;
    bool has_previous_position_{};
    std::deque<cv::Point2f> position_history_;
    int missed_detection_frames_{};
    int foreground_confirmed_frames_{};
    cv::Mat background_model_;
    bool background_initialized_{};
};

#endif  // FOOSBALL_TRACKER_APP_MODEL_INCLUDE_PRIVATE_DETECTORS_BALL_DETECTOR_HPP_
