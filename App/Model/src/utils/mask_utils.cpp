#include "mask_utils.hpp"

#include <opencv2/core.hpp>
#include <opencv2/core/mat.inl.hpp>
#include <opencv2/core/traits.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>

#include "detector_types.hpp"

namespace mask_utils
{

cv::Mat create_kernel(const int size, const int shape)
{
    return cv::getStructuringElement(shape, cv::Size{size, size});
}

cv::Mat build_hsv_mask(const cv::Mat& frame, const cv::Scalar& lower, const cv::Scalar& upper)
{
    cv::Mat hsv;
    cv::Mat mask;

    cv::cvtColor(frame, hsv, cv::COLOR_BGR2HSV);
    cv::inRange(hsv, lower, upper, mask);

    return mask;
}

void draw_label(cv::Mat& frame,
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

void write_mask_if_verbose(const std::string& path, const cv::Mat& mask)
{
    if (!detector_types::kVerbose)
    {
        return;
    }

    cv::imwrite(path, mask);
}

}  // namespace mask_utils