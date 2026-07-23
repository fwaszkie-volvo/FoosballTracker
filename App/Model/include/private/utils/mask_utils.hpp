#ifndef FOOSBALL_TRACKER_APP_MODEL_INCLUDE_PRIVATE_UTILS_MASK_UTILS_HPP_
#define FOOSBALL_TRACKER_APP_MODEL_INCLUDE_PRIVATE_UTILS_MASK_UTILS_HPP_

#include <opencv2/core/mat.hpp>
#include <opencv2/core/types.hpp>
#include <string>

class MaskUtils
{
  public:
    struct HsvRange
    {
        cv::Scalar lower;
        cv::Scalar upper;
    };

    static cv::Mat CreateKernel(const int size, const int shape);
    static cv::Mat BuildHsvMask(const cv::Mat &frame,
                                const cv::Scalar &lower,
                                const cv::Scalar &upper);
    static cv::Mat BuildDualHsvMask(const cv::Mat &frame,
                                    const HsvRange &range1,
                                    const HsvRange &range2);
    static void OpenMask(cv::Mat &mask, int kernel_size);
    static void DrawLabel(cv::Mat &frame,
                          const std::string &label,
                          const cv::Point &anchor,
                          const cv::Scalar &color);
    static void WriteMaskIfVerbose(const char *path, const cv::Mat &mask);
};

#endif  // FOOSBALL_TRACKER_APP_MODEL_INCLUDE_PRIVATE_UTILS_MASK_UTILS_HPP_
