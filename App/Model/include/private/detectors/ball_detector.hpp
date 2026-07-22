#ifndef FOOSBALL_TRACKER_APP_MODEL_INCLUDE_PRIVATE_DETECTORS_BALL_DETECTOR_HPP_
#define FOOSBALL_TRACKER_APP_MODEL_INCLUDE_PRIVATE_DETECTORS_BALL_DETECTOR_HPP_

#include <opencv2/opencv.hpp>
#include <vector>

#include "detector.hpp"
#include "playfield_detector.hpp"

class BallDetector : public Detector
{
  public:
    void Detect(const cv::Mat& frame) override;
    void Draw(cv::Mat& frame) const override;

  private:
    struct DetectionCandidate
    {
        bool found = false;
        cv::Point center;
        int radius = 0;
        double score = -1.0;
    };

    bool IsCircleInsideFrame(const cv::Point& center, const int radius, const cv::Size& size) const;
    void ResetBestCandidate();
    void UpdateCandidate(const cv::Point& center, const int radius, const double score);
    void CollectHoughCandidate(const cv::Mat& frame,
                               const cv::Mat& mask,
                               const cv::Point& center,
                               const int radius);
    void CollectContourCandidate(const cv::Mat& frame, const std::vector<cv::Point>& contour);
    cv::Mat BuildForegroundMask(const cv::Mat& gray, const cv::Mat& playfield_mask);

    PlayfieldDetector playfield_detector_;
    DetectionCandidate best_candidate_;
    cv::Mat background_model_;
    bool background_initialized_ = false;
};

#endif  // FOOSBALL_TRACKER_APP_MODEL_INCLUDE_PRIVATE_DETECTORS_BALL_DETECTOR_HPP_
