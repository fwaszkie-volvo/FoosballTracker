#include "mask_utils.hpp"

#include <opencv2/core.hpp>
#include <opencv2/core/mat.inl.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>

#include "detector_config.hpp"

cv::Mat MaskUtils::CreateKernel(const int size, const int shape)
{
    return cv::getStructuringElement(shape, cv::Size(size, size));
}

cv::Mat MaskUtils::BuildHsvMask(const cv::Mat &frame,
                                const cv::Scalar &lower,
                                const cv::Scalar &upper)
{
    cv::Mat hsv;
    cv::Mat mask;

    cv::cvtColor(frame, hsv, cv::COLOR_BGR2HSV);
    cv::inRange(hsv, lower, upper, mask);

    return mask;
}

cv::Mat MaskUtils::BuildDualHsvMask(const cv::Mat &frame,
                                    const HsvRange &range1,
                                    const HsvRange &range2)
{
    cv::Mat mask1 = BuildHsvMask(frame, range1.lower, range1.upper);
    cv::Mat mask2 = BuildHsvMask(frame, range2.lower, range2.upper);
    cv::Mat mask;

    cv::bitwise_or(mask1, mask2, mask);
    return mask;
}

void MaskUtils::OpenMask(cv::Mat &mask, int kernel_size)
{
    cv::Mat kernel = CreateKernel(kernel_size, cv::MORPH_RECT);
    cv::morphologyEx(mask, mask, cv::MORPH_OPEN, kernel);
}

void MaskUtils::DrawLabel(cv::Mat &frame,
                          const std::string &label,
                          const cv::Point &anchor,
                          const cv::Scalar &color)
{
    cv::putText(frame,
                label,
                cv::Point(anchor.x, anchor.y - detector_config::kLabelYOffset),
                cv::FONT_HERSHEY_SIMPLEX,
                detector_config::kLabelScale,
                color,
                detector_config::kDrawThickness);
}

void MaskUtils::WriteMaskIfVerbose(const char *path, const cv::Mat &mask)
{
    if (!detector_config::kVerbose)
    {
        return;
    }

    cv::imwrite(path, mask);
}
