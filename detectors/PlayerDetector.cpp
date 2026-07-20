#include "PlayerDetector.hpp"

#include <vector>

#include "MaskUtils.hpp"
#include "detector_config.hpp"

PlayerDetector::PlayerDetector(const cv::Scalar &lower,
                               const cv::Scalar &upper,
                               const cv::Scalar &draw_color,
                               const std::string &label)
    : dual_range_(false), lower1_(lower), upper1_(upper), lower2_(), upper2_(), draw_color_(draw_color), label_(label)
{
}

PlayerDetector::PlayerDetector(const cv::Scalar &lower1,
                               const cv::Scalar &upper1,
                               const cv::Scalar &lower2,
                               const cv::Scalar &upper2,
                               const cv::Scalar &draw_color,
                               const std::string &label)
    : dual_range_(true), lower1_(lower1), upper1_(upper1), lower2_(lower2), upper2_(upper2), draw_color_(draw_color), label_(label)
{
}

void PlayerDetector::DrawDetections(cv::Mat &frame, const cv::Mat &mask) const
{
    std::vector<std::vector<cv::Point>> contours;
    std::vector<cv::Vec4i> hierarchy;
    cv::findContours(mask, contours, hierarchy, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

    for (const auto &contour : contours)
    {
        if (cv::contourArea(contour) <= detector_config::kPlayerMinArea)
        {
            continue;
        }

        cv::Rect bounding_box = cv::boundingRect(contour);
        cv::rectangle(frame, bounding_box, draw_color_, detector_config::kDrawThickness);
        MaskUtils::DrawLabel(frame, label_, bounding_box.tl(), draw_color_);
    }
}

cv::Mat PlayerDetector::Detect(cv::Mat &frame) const
{
    cv::Mat mask = dual_range_ ? MaskUtils::BuildDualHsvMask(frame, lower1_, upper1_, lower2_, upper2_)
                               : MaskUtils::BuildHsvMask(frame, lower1_, upper1_);
    MaskUtils::OpenMask(mask, detector_config::kPlayerKernelSize);
    DrawDetections(frame, mask);
    return frame;
}

cv::Mat detect_blue(cv::Mat &frame)
{
    static const PlayerDetector detector(detector_config::kLowerBlue,
                                         detector_config::kUpperBlue,
                                         detector_config::kBlueDrawColor,
                                         "Blue Player");
    return detector.Detect(frame);
}

cv::Mat detect_red(cv::Mat &frame)
{
    static const PlayerDetector detector(detector_config::kLowerRed1,
                                         detector_config::kUpperRed1,
                                         detector_config::kLowerRed2,
                                         detector_config::kUpperRed2,
                                         detector_config::kRedDrawColor,
                                         "Red Player");
    return detector.Detect(frame);
}
