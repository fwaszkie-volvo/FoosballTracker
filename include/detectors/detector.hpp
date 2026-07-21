#ifndef DETECTOR_HPP_
#define DETECTOR_HPP_

#include <opencv2/opencv.hpp>

#include "ball-detector.hpp"

cv::Mat detect_ball(cv::Mat& frame);

#endif /* DETECTOR_HPP_ */
