#include "playfield_detector.hpp"

#include <cmath>
#include <vector>

#include "detector_types.hpp"
#include "mask_utils.hpp"

bool PlayfieldDetector::ChooseLargestContour(const std::vector<std::vector<cv::Point>>& contours,
                                             std::vector<cv::Point>& largest_contour) const
{
    double best_area{0.0};

    for (const auto& contour : contours)
    {
        const double area{cv::contourArea(contour)};
        if (area > best_area)
        {
            best_area = area;
            largest_contour = contour;
        }
    }

    return !largest_contour.empty();
}

std::vector<cv::Point> PlayfieldDetector::ApproximatePolygon(
  const std::vector<cv::Point>& hull) const
{
    std::vector<cv::Point> best_polygon{hull};
    int best_vertex_delta{
      std::abs(static_cast<int>(hull.size()) - detector_types::kPlayfieldTargetVertices)};
    const double hull_perimeter{cv::arcLength(hull, true)};
    const int epsilon_steps{static_cast<int>(
      std::lround((detector_types::kPlayfieldApproxEnd - detector_types::kPlayfieldApproxStart) /
                  detector_types::kPlayfieldApproxStep))};

    for (int epsilon_step{0}; epsilon_step <= epsilon_steps; ++epsilon_step)
    {
        const double epsilon_scale{detector_types::kPlayfieldApproxStart +
                                   (epsilon_step * detector_types::kPlayfieldApproxStep)};
        std::vector<cv::Point> polygon;
        cv::approxPolyDP(hull, polygon, epsilon_scale * hull_perimeter, true);

        if (polygon.size() < 4)
        {
            continue;
        }

        const int vertex_delta{
          std::abs(static_cast<int>(polygon.size()) - detector_types::kPlayfieldTargetVertices)};
        if (vertex_delta < best_vertex_delta)
        {
            best_vertex_delta = vertex_delta;
            best_polygon = polygon;
        }

        if (polygon.size() == detector_types::kPlayfieldTargetVertices)
        {
            return polygon;
        }
    }

    return best_polygon;
}

void PlayfieldDetector::Detect(const cv::Mat& frame)
{
    detected_ = false;
    playfield_polygon_.clear();
    playfield_mask_.release();

    cv::Mat green_mask{
      MaskUtils::BuildHsvMask(frame, detector_types::kLowerGreen, detector_types::kUpperGreen)};
    cv::Mat green_dominance_mask;

    std::vector<cv::Mat> bgr_channels;
    cv::split(frame, bgr_channels);

    cv::Mat green_minus_red;
    cv::Mat green_minus_blue;
    cv::subtract(bgr_channels[1], bgr_channels[2], green_minus_red);
    cv::subtract(bgr_channels[1], bgr_channels[0], green_minus_blue);

    cv::Mat green_over_red_mask;
    cv::Mat green_over_blue_mask;
    cv::threshold(green_minus_red,
                  green_over_red_mask,
                  detector_types::kGreenDominanceThreshold,
                  255,
                  cv::THRESH_BINARY);
    cv::threshold(green_minus_blue,
                  green_over_blue_mask,
                  detector_types::kGreenDominanceThreshold,
                  255,
                  cv::THRESH_BINARY);
    cv::bitwise_and(green_over_red_mask, green_over_blue_mask, green_dominance_mask);
    cv::bitwise_and(green_mask, green_dominance_mask, green_mask);

    cv::Mat kernel{
      MaskUtils::CreateKernel(detector_types::kPlayfieldKernelSize, cv::MORPH_ELLIPSE)};
    cv::morphologyEx(green_mask, green_mask, cv::MORPH_CLOSE, kernel);
    cv::morphologyEx(green_mask, green_mask, cv::MORPH_OPEN, kernel);
    cv::dilate(green_mask,
               green_mask,
               kernel,
               cv::Point(-1, -1),
               detector_types::kPlayfieldDilateIterations);

    std::vector<std::vector<cv::Point>> contours;
    std::vector<cv::Vec4i> hierarchy;
    cv::findContours(green_mask, contours, hierarchy, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

    std::vector<cv::Point> largest_contour;
    if (!ChooseLargestContour(contours, largest_contour))
    {
        return;
    }

    std::vector<cv::Point> hull;
    cv::convexHull(largest_contour, hull);

    playfield_mask_ = cv::Mat::zeros(frame.size(), CV_8UC1);
    cv::fillConvexPoly(playfield_mask_, hull, cv::Scalar(255));
    playfield_polygon_ = ApproximatePolygon(hull);
    detected_ = true;
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
