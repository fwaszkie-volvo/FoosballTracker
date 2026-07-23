#ifndef FOOSBALL_TRACKER_APP_MODEL_INCLUDE_PRIVATE_UTILS_MASK_UTILS_HPP_
#define FOOSBALL_TRACKER_APP_MODEL_INCLUDE_PRIVATE_UTILS_MASK_UTILS_HPP_

#include <opencv2/core/mat.hpp>
#include <opencv2/core/types.hpp>
#include <string>

namespace mask_utils
{
cv::Mat create_kernel(const int size, const int shape);
cv::Mat build_hsv_mask(const cv::Mat& frame, const cv::Scalar& lower, const cv::Scalar& upper);
void draw_label(cv::Mat& frame,
                const std::string& label,
                const cv::Point& anchor,
                const cv::Scalar& color);
void write_mask_if_verbose(const std::string& path, const cv::Mat& mask);
};  // namespace mask_utils

#endif  // FOOSBALL_TRACKER_APP_MODEL_INCLUDE_PRIVATE_UTILS_MASK_UTILS_HPP_
