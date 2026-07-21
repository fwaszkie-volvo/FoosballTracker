#ifndef FOOSBALL_TRACKER_DETECTOR_HPP_
#define FOOSBALL_TRACKER_DETECTOR_HPP_

#include <opencv2/opencv.hpp>

#include "ball_detector.hpp"

cv::Mat detect_ball(cv::Mat& frame);

#endif  // FOOSBALL_TRACKER_DETECTOR_HPP_
