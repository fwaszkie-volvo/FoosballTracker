#ifndef FOOSBALL_TRACKER_APP_MODEL_INCLUDE_PRIVATE_UTILS_MASK_UTILS_HPP_
#define FOOSBALL_TRACKER_APP_MODEL_INCLUDE_PRIVATE_UTILS_MASK_UTILS_HPP_

#include <opencv2/opencv.hpp>
#include <string>

class MaskUtils
{
  public:
    static cv::Mat CreateKernel(int size, int shape);
    static cv::Mat BuildHsvMask(const cv::Mat &frame,
                                const cv::Scalar &lower,
                                const cv::Scalar &upper);
    static cv::Mat BuildDualHsvMask(const cv::Mat &frame,
                                    const cv::Scalar &lower1,
                                    const cv::Scalar &upper1,
                                    const cv::Scalar &lower2,
                                    const cv::Scalar &upper2);
    static void OpenMask(cv::Mat &mask, int kernel_size);
    static void DrawLabel(cv::Mat &frame,
                          const std::string &label,
                          const cv::Point &anchor,
                          const cv::Scalar &color);
    static void WriteMaskIfVerbose(const char *path, const cv::Mat &mask);
};

#endif  // FOOSBALL_TRACKER_APP_MODEL_INCLUDE_PRIVATE_UTILS_MASK_UTILS_HPP_
