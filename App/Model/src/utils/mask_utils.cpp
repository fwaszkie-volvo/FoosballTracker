#include "mask_utils.hpp"

#include "detector_types.hpp"

cv::Mat MaskUtils::CreateKernel(int size, int shape)
{
    return cv::getStructuringElement(shape, cv::Size{size, size});
}

cv::Mat MaskUtils::BuildHsvMask(const cv::Mat& frame,
                                const cv::Scalar& lower,
                                const cv::Scalar& upper)
{
    cv::Mat hsv;
    cv::Mat mask;

    cv::cvtColor(frame, hsv, cv::COLOR_BGR2HSV);
    cv::inRange(hsv, lower, upper, mask);

    return mask;
}

void MaskUtils::DrawLabel(cv::Mat& frame,
                          const std::string& label,
                          const cv::Point& anchor,
                          const cv::Scalar& color)
{
    cv::putText(frame,
                label,
                cv::Point(anchor.x, anchor.y - detector_types::kLabelYOffset),
                cv::FONT_HERSHEY_SIMPLEX,
                detector_types::kLabelScale,
                color,
                detector_types::kDrawThickness);
}

void MaskUtils::WriteMaskIfVerbose(const std::string& path, const cv::Mat& mask)
{
    if (!detector_types::kVerbose)
    {
        return;
    }

    cv::imwrite(path, mask);
}
