#include "playfield_detector.hpp"

#include <bits/std_abs.h>
#include <opencv2/core/hal/interface.h>

#include <cmath>
#include <cstdlib>
#include <opencv2/core.hpp>
#include <opencv2/core/mat.hpp>
#include <opencv2/core/mat.inl.hpp>
#include <opencv2/core/matx.hpp>
#include <opencv2/core/traits.hpp>
#include <opencv2/core/types.hpp>
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
    Contour best_polygon{hull};
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
        Contour polygon;
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
            best_polygon      = polygon;
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
    if (detected_)
    {
        return;
    }

    playfield_polygon_.clear();
    playfield_mask_.release();

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

    std::vector<Contour> contours;
    std::vector<cv::Vec4i> hierarchy;
    cv::findContours(green_mask, contours, hierarchy, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

    Contour largest_contour;
    if (!ChooseLargestContour(contours, largest_contour))
    {
        return;
    }

    const double minimum_contour_area{cv::contourArea(largest_contour) *
                                      detector_types::kPlayfieldContourMinAreaRatio};
    cv::Mat column_coverage;
    cv::reduce(green_mask, column_coverage, 0, cv::REDUCE_SUM, CV_32S);
    double maximum_column_coverage{};
    cv::minMaxLoc(column_coverage, nullptr, &maximum_column_coverage);
    const double minimum_column_coverage{maximum_column_coverage *
                                         detector_types::kPlayfieldColumnMinCoverageRatio};

    Contour playfield_points;
    for (const auto& contour : contours)
    {
        if (cv::contourArea(contour) >= minimum_contour_area)
        {
            for (const auto& point : contour)
            {
                if (column_coverage.at<int>(0, point.x) >= minimum_column_coverage)
                {
                    playfield_points.push_back(point);
                }
            }
        }
    }

    if (playfield_points.empty())
    {
        return;
    }

    Contour hull{};
    cv::convexHull(playfield_points, hull);
    const double frame_area{static_cast<double>(frame.rows) * static_cast<double>(frame.cols)};
    if (cv::contourArea(hull) < frame_area * detector_types::kPlayfieldMinFrameAreaRatio)
    {
        return;
    }

    playfield_mask_ = cv::Mat::zeros(frame.size(), CV_8UC1);
    cv::fillConvexPoly(playfield_mask_, hull, cv::Scalar(255));
    playfield_polygon_ = ApproximatePolygon(hull);
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
