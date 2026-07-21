#ifndef FOOSBALL_TRACKER_BALL_DETECTOR_HPP_
#define FOOSBALL_TRACKER_BALL_DETECTOR_HPP_

#include <opencv2/opencv.hpp>
#include <vector>

#include "playfield_detector.hpp"

class BallDetector
{
  public:
    cv::Mat Detect(cv::Mat& frame);

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
    void DrawDetection(cv::Mat& frame) const;

    PlayfieldDetector playfield_detector_;
    DetectionCandidate best_candidate_;
    cv::Mat background_model_;
    bool background_initialized_ = false;
};

cv::Mat detect_ball(cv::Mat& frame);

#endif  // FOOSBALL_TRACKER_BALL_DETECTOR_HPP_
