#ifndef FOOSBALL_TRACKER_APP_MODEL_INCLUDE_PRIVATE_DETECTORS_DETECTOR_HPP_
#define FOOSBALL_TRACKER_APP_MODEL_INCLUDE_PRIVATE_DETECTORS_DETECTOR_HPP_

#include <opencv2/opencv.hpp>

class Detector
{
  public:
    virtual ~Detector() = default;
    virtual void Detect(const cv::Mat& frame) = 0;
    virtual void Draw(cv::Mat& frame) const = 0;
};

cv::Mat detect_ball(cv::Mat& frame);

#endif  // FOOSBALL_TRACKER_APP_MODEL_INCLUDE_PRIVATE_DETECTORS_DETECTOR_HPP_
