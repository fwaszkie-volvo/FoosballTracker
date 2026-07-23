#ifndef FOOSBALL_TRACKER_APP_MODEL_INCLUDE_PRIVATE_DETECTORS_BALL_DETECTOR_HPP_
#define FOOSBALL_TRACKER_APP_MODEL_INCLUDE_PRIVATE_DETECTORS_BALL_DETECTOR_HPP_

#include <opencv2/core/mat.hpp>
#include <opencv2/core/types.hpp>
#include <vector>

#include "playfield_detector.hpp"

class BallDetector
{
  public:
    cv::Mat Detect(cv::Mat &frame);

  private:
    struct CandidateMetrics
    {
        int radius = 0;
        double score = -1.0;
    };

    struct DetectionCandidate
    {
        bool found = false;
        cv::Point center;
        int radius = 0;
        double score = -1.0;
    };

    bool IsCircleInsideFrame(const cv::Point &center, int radius, const cv::Size &size) const;
    void ResetBestCandidate();
    void UpdateCandidate(const cv::Point &center, const CandidateMetrics &metrics);
    void CollectHoughCandidate(const cv::Mat &frame,
                               const cv::Mat &mask,
                               const cv::Point &center,
                               int radius);
    void CollectContourCandidate(const cv::Mat &frame, const std::vector<cv::Point> &contour);
    void DrawDetection(cv::Mat &frame) const;

    PlayfieldDetector playfield_detector_;
    DetectionCandidate best_candidate_;
};

cv::Mat detect_ball(cv::Mat &frame);

#endif  // FOOSBALL_TRACKER_APP_MODEL_INCLUDE_PRIVATE_DETECTORS_BALL_DETECTOR_HPP_
