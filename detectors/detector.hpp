#ifndef DETECTOR_HPP_
#define DETECTOR_HPP_

#include <opencv2/opencv.hpp>

#include "BallDetector.hpp"
#include "PlayerDetector.hpp"

cv::Mat detect_blue(cv::Mat &frame);
cv::Mat detect_red(cv::Mat &frame);
cv::Mat detect_ball(cv::Mat &frame);

#endif /* DETECTOR_HPP_ */
