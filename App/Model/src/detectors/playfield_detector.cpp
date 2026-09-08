#include "playfield_detector.hpp"

#include <opencv2/core/hal/interface.h>

#include <opencv2/core.hpp>
#include <opencv2/core/base.hpp>
#include <opencv2/core/mat.inl.hpp>
#include <opencv2/core/traits.hpp>
#include <opencv2/imgproc.hpp>
#include <vector>

#include "detector_types.hpp"
#include "mask_utils.hpp"

bool PlayfieldDetector::ChooseLargestContour(const std::vector<Contour>& contours,
                                             Contour& largest_contour) const
{
    double best_area{0.0};

    for (const auto& contour : contours)
    {
        const double area{cv::contourArea(contour)};
        if (area > best_area)
        {
            best_area       = area;
            largest_contour = contour;
        }
    }

    return !largest_contour.empty();
}

Contour PlayfieldDetector::ApproximatePolygon(const Contour& hull) const
{
    const double hull_perimeter{cv::arcLength(hull, true)};
    Contour polygon;
    cv::approxPolyDP(
      hull, polygon, detector_types::kPlayfieldApproximationRatio * hull_perimeter, true);

    return polygon.size() >= 4 ? polygon : hull;
}

cv::Mat PlayfieldDetector::BuildGreenMask(const cv::Mat& frame) const
{
    cv::Mat green_mask{
      mask_utils::build_hsv_mask(frame, detector_types::kLowerGreen, detector_types::kUpperGreen)};
    cv::Mat green_dominance_mask;
    std::vector<cv::Mat> bgr_channels;
    cv::split(frame, bgr_channels);

    cv::Mat green_channel;
    cv::Mat scaled_red_channel;
    cv::Mat scaled_blue_channel;
    bgr_channels[1].convertTo(green_channel, CV_32F);
    bgr_channels[2].convertTo(scaled_red_channel, CV_32F, detector_types::kGreenDominanceRatio);
    bgr_channels[0].convertTo(scaled_blue_channel, CV_32F, detector_types::kGreenDominanceRatio);

    cv::Mat green_over_red_mask;
    cv::Mat green_over_blue_mask;
    cv::compare(green_channel, scaled_red_channel, green_over_red_mask, cv::CMP_GT);
    cv::compare(green_channel, scaled_blue_channel, green_over_blue_mask, cv::CMP_GT);
    cv::bitwise_and(green_over_red_mask, green_over_blue_mask, green_dominance_mask);
    cv::bitwise_and(green_mask, green_dominance_mask, green_mask);

    cv::Mat kernel{
      mask_utils::create_kernel(detector_types::kPlayfieldKernelSize, cv::MORPH_ELLIPSE)};
    cv::morphologyEx(green_mask, green_mask, cv::MORPH_CLOSE, kernel);
    cv::morphologyEx(green_mask, green_mask, cv::MORPH_OPEN, kernel);
    cv::Mat horizontal_kernel{cv::getStructuringElement(
      cv::MORPH_RECT, detector_types::kPlayfieldHorizontalCloseKernelSize)};
    cv::morphologyEx(green_mask, green_mask, cv::MORPH_CLOSE, horizontal_kernel);
    return green_mask;
}

bool PlayfieldDetector::SelectPlayfieldPoints(const cv::Mat& green_mask,
                                              const Contour& largest_contour,
                                              Contour& playfield_points) const
{
    cv::Mat column_coverage;
    cv::reduce(green_mask, column_coverage, 0, cv::REDUCE_SUM, CV_32S);
    double maximum_column_coverage{};
    cv::minMaxLoc(column_coverage, nullptr, &maximum_column_coverage);
    const double minimum_column_coverage{maximum_column_coverage *
                                         detector_types::kPlayfieldColumnMinCoverageRatio};
    for (const auto& point : largest_contour)
    {
        if (column_coverage.at<int>(0, point.x) >= minimum_column_coverage)
        {
            playfield_points.push_back(point);
        }
    }
    return !playfield_points.empty();
}

bool PlayfieldDetector::IsValidPlayfield(const Contour& playfield_contour,
                                         const cv::Size& frame_size) const
{
    const double frame_area{static_cast<double>(frame_size.height) * frame_size.width};
    const cv::Rect hull_bounds{cv::boundingRect(playfield_contour)};
    return cv::contourArea(playfield_contour) >=
             frame_area * detector_types::kPlayfieldMinFrameAreaRatio &&
           hull_bounds.width >= frame_size.width * detector_types::kPlayfieldMinFrameWidthRatio &&
           hull_bounds.height >= frame_size.height * detector_types::kPlayfieldMinFrameHeightRatio;
}

void PlayfieldDetector::Detect(const cv::Mat& frame)
{
    if (detected_)
    {
        return;
    }

    playfield_polygon_.clear();
    playfield_mask_.release();

    cv::Mat green_mask{BuildGreenMask(frame)};

    std::vector<Contour> contours;
    cv::findContours(green_mask, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

    Contour largest_contour;
    if (!ChooseLargestContour(contours, largest_contour))
    {
        return;
    }

    Contour playfield_points;
    if (!SelectPlayfieldPoints(green_mask, largest_contour, playfield_points))
    {
        return;
    }

    Contour playfield_contour;
    cv::convexHull(playfield_points, playfield_contour);

    if (!IsValidPlayfield(playfield_contour, frame.size()))
    {
        return;
    }

    playfield_mask_ = cv::Mat::zeros(frame.size(), CV_8UC1);
    cv::fillPoly(playfield_mask_, std::vector<Contour>{playfield_contour}, cv::Scalar(255));
    playfield_polygon_ = ApproximatePolygon(playfield_contour);
    detected_          = true;
}

void PlayfieldDetector::Draw(cv::Mat& frame) const
{
    if (!detected_ || playfield_polygon_.empty())
    {
        return;
    }

    cv::polylines(frame,
                  playfield_polygon_,
                  true,
                  detector_types::kPlayfieldDrawColor,
                  detector_types::kDrawThickness);
}

void PlayfieldDetector::Reset()
{
    detected_ = false;
    playfield_polygon_.clear();
    playfield_mask_.release();
}
