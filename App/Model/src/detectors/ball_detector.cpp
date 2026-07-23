#include "ball_detector.hpp"

#include <opencv2/core/cvdef.h>
#include <opencv2/core/hal/interface.h>

#include <cmath>
#include <opencv2/core.hpp>
#include <opencv2/core/mat.inl.hpp>
#include <opencv2/core/matx.hpp>
#include <opencv2/imgproc.hpp>
#include <vector>

#include "detector_types.hpp"
#include "mask_utils.hpp"

bool BallDetector::IsCircleInsideFrame(const cv::Point& center,
                                       const int radius,
                                       const cv::Size& size) const
{
    return center.x - radius >= 0 && center.y - radius >= 0 && center.x + radius < size.width &&
           center.y + radius < size.height;
}

void BallDetector::ResetBestCandidate() { best_candidate_ = DetectionCandidate{}; }

<<<<<<< HEAD
void BallDetector::UpdateCandidate(const cv::Point &center, const CandidateMetrics &metrics)
=======
void BallDetector::UpdateCandidate(const cv::Point& center, const int radius, const double score)
>>>>>>> origin/master
{
    if (metrics.score <= best_candidate_.score)
    {
        return;
    }

    best_candidate_.found = true;
    best_candidate_.center = center;
    best_candidate_.radius = metrics.radius;
    best_candidate_.score = metrics.score;
}

void BallDetector::CollectHoughCandidate(const cv::Mat& frame,
                                         const cv::Mat& mask,
                                         const cv::Point& center,
                                         const int radius)
{
    if (!IsCircleInsideFrame(center, radius, frame.size()))
    {
        return;
    }

    cv::Mat circle_mask{cv::Mat::zeros(mask.size(), CV_8UC1)};
    cv::circle(
      circle_mask, center, radius, cv::Scalar(detector_types::kWhitePixelValue), cv::FILLED);

    cv::Mat white_overlap;
    cv::bitwise_and(mask, circle_mask, white_overlap);

    const double circle_area{static_cast<double>(cv::countNonZero(circle_mask))};
    if (circle_area <= 0.0)
    {
        return;
    }

    const double white_ratio{static_cast<double>(cv::countNonZero(white_overlap)) / circle_area};
    if (white_ratio < detector_types::kCircleMinWhiteRatio)
    {
        return;
    }

    const cv::Scalar mean_bgr{cv::mean(frame, circle_mask)};
    const double brightness{
      (mean_bgr[0] + mean_bgr[1] + mean_bgr[2]) /
      (detector_types::kColorChannels * detector_types::kBrightnessNormalization)};
    const double score{white_ratio * detector_types::kCircleWhiteRatioWeight +
                       brightness * detector_types::kCircleBrightnessWeight};

    UpdateCandidate(center, CandidateMetrics{.radius = radius, .score = score});
}

void BallDetector::CollectContourCandidate(const cv::Mat& frame,
                                           const std::vector<cv::Point>& contour)
{
    const double area{cv::contourArea(contour)};
    if (area < detector_types::kBallMinArea || area > detector_types::kBallMaxArea)
    {
        return;
    }

    const double perimeter{cv::arcLength(contour, true)};
    if (perimeter <= 0.0)
    {
        return;
    }

    const double circularity{4.0 * CV_PI * area / (perimeter * perimeter)};
    if (circularity < detector_types::kBallMinCircularity)
    {
        return;
    }

    cv::Point2f center_f;
    float radius_f;
    cv::minEnclosingCircle(contour, center_f, radius_f);
    if (radius_f < detector_types::kBallMinRadius || radius_f > detector_types::kBallMaxRadius)
    {
        return;
    }

    const cv::Rect box{cv::boundingRect(contour)};
    if (box.x <= detector_types::kEdgeThreshold || box.y <= detector_types::kEdgeThreshold ||
        box.x + box.width >= frame.cols - detector_types::kEdgeThreshold ||
        box.y + box.height >= frame.rows - detector_types::kEdgeThreshold)
    {
        return;
    }

    const double aspect_ratio{static_cast<double>(box.width) / static_cast<double>(box.height)};
    if (aspect_ratio < detector_types::kBallMinAspectRatio ||
        aspect_ratio > detector_types::kBallMaxAspectRatio)
    {
        return;
    }

    const double extent{area / static_cast<double>(box.area())};
    if (extent < detector_types::kBallMinExtent || extent > detector_types::kBallMaxExtent)
    {
        return;
    }

<<<<<<< HEAD
    cv::Point center(static_cast<int>(std::round(center_f.x)),
                     static_cast<int>(std::round(center_f.y)));
    int radius = static_cast<int>(std::round(radius_f));
    UpdateCandidate(center, CandidateMetrics{.radius = radius, .score = circularity * extent});
=======
    const cv::Point center{static_cast<int>(std::round(center_f.x)),
                           static_cast<int>(std::round(center_f.y))};
    const int radius{static_cast<int>(std::round(radius_f))};
    UpdateCandidate(center, radius, circularity * extent);
>>>>>>> origin/master
}

cv::Mat BallDetector::BuildForegroundMask(const cv::Mat& gray, const cv::Mat& playfield_mask)
{
    if (!background_initialized_ || background_model_.empty() ||
        background_model_.size() != gray.size() || background_model_.type() != CV_32FC1)
    {
        gray.convertTo(background_model_, CV_32FC1);
        background_initialized_ = true;
        return cv::Mat{};
    }

    cv::Mat background_u8;
    background_model_.convertTo(background_u8, CV_8UC1);

    cv::Mat diff;
    cv::absdiff(gray, background_u8, diff);
    mask_utils::write_mask_if_verbose(detector_types::kBackgroundDiffPath, diff);

    cv::Mat foreground_mask;
    cv::threshold(diff,
                  foreground_mask,
                  detector_types::kBackgroundDiffThreshold,
                  detector_types::kWhitePixelValue,
                  cv::THRESH_BINARY);

    cv::Mat foreground_kernel =
      mask_utils::create_kernel(detector_types::kForegroundKernelSize, cv::MORPH_ELLIPSE);
    cv::morphologyEx(foreground_mask, foreground_mask, cv::MORPH_OPEN, foreground_kernel);
    cv::morphologyEx(foreground_mask, foreground_mask, cv::MORPH_CLOSE, foreground_kernel);

    if (!playfield_mask.empty())
    {
        cv::bitwise_and(foreground_mask, playfield_mask, foreground_mask);
        cv::accumulateWeighted(
          gray, background_model_, detector_types::kBackgroundLearningRate, playfield_mask);
    }
    else
    {
        cv::accumulateWeighted(gray, background_model_, detector_types::kBackgroundLearningRate);
    }

    return foreground_mask;
}

void BallDetector::Draw(cv::Mat& frame) const
{
    playfield_detector_.Draw(frame);

    if (!best_candidate_.found)
    {
        return;
    }

    cv::circle(frame,
               best_candidate_.center,
               best_candidate_.radius,
               detector_types::kBallDrawColor,
               detector_types::kDrawThickness);
    mask_utils::draw_label(frame, "Ball", best_candidate_.center, detector_types::kBallDrawColor);
}

void BallDetector::Detect(const cv::Mat& frame)
{
    cv::Mat mask{
      mask_utils::build_hsv_mask(frame, detector_types::kLowerWhite, detector_types::kUpperWhite)};
    cv::Mat ball_kernel{
      mask_utils::create_kernel(detector_types::kBallKernelSize, cv::MORPH_ELLIPSE)};
    cv::morphologyEx(mask, mask, cv::MORPH_OPEN, ball_kernel);
    cv::morphologyEx(mask, mask, cv::MORPH_CLOSE, ball_kernel);

    playfield_detector_.Detect(frame);
    const cv::Mat& playfield_mask{playfield_detector_.GetMask()};
    if (playfield_detector_.HasDetection())
    {
        mask_utils::write_mask_if_verbose(detector_types::kFieldMaskPath, playfield_mask);
        cv::bitwise_and(mask, playfield_mask, mask);
    }

    cv::Mat color_mask{mask.clone()};

    cv::Mat gray;
    cv::cvtColor(frame, gray, cv::COLOR_BGR2GRAY);
    cv::GaussianBlur(gray,
                     gray,
                     detector_types::kGaussianBlurKernelSize,
                     detector_types::kGaussianBlurSigma,
                     detector_types::kGaussianBlurSigma);
    if (!playfield_mask.empty())
    {
        cv::bitwise_and(gray, playfield_mask, gray);
    }

    cv::Mat foreground_mask{BuildForegroundMask(gray, playfield_mask)};
    if (!foreground_mask.empty())
    {
        mask_utils::write_mask_if_verbose(detector_types::kForegroundMaskPath, foreground_mask);

        cv::Mat motion_refined_mask;
        cv::bitwise_and(color_mask, foreground_mask, motion_refined_mask);

        if (cv::countNonZero(motion_refined_mask) >= detector_types::kForegroundMinPixels)
        {
            mask = motion_refined_mask;
        }
        else
        {
            mask = color_mask;
        }
    }
    else
    {
        mask = color_mask;
    }

    mask_utils::write_mask_if_verbose(detector_types::kBallMaskPath, mask);

    ResetBestCandidate();

    std::vector<cv::Vec3f> circles;
    cv::HoughCircles(gray,
                     circles,
                     cv::HOUGH_GRADIENT,
                     detector_types::kHoughDp,
                     detector_types::kHoughMinDist,
                     detector_types::kHoughParam1,
                     detector_types::kHoughParam2,
                     detector_types::kHoughMinRadius,
                     detector_types::kHoughMaxRadius);

    for (const auto& circle : circles)
    {
        cv::Point center{static_cast<int>(std::round(circle[0])),
                         static_cast<int>(std::round(circle[1]))};
        int radius{static_cast<int>(std::round(circle[2]))};
        CollectHoughCandidate(frame, mask, center, radius);
    }

    std::vector<std::vector<cv::Point>> contours;
    std::vector<cv::Vec4i> hierarchy;
    cv::findContours(mask, contours, hierarchy, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

    for (const auto& contour : contours)
    {
        CollectContourCandidate(frame, contour);
    }
}

cv::Mat detect_ball(cv::Mat& frame)
{
    static BallDetector detector;
    detector.Detect(frame);
    detector.Draw(frame);
    return frame;
}
