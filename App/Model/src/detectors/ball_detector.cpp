#include "ball_detector.hpp"

#include <cmath>
#include <vector>

#include "detector_config.hpp"
#include "mask_utils.hpp"

bool BallDetector::IsCircleInsideFrame(const cv::Point& center,
                                       const int radius,
                                       const cv::Size& size) const
{
    return center.x - radius >= 0 && center.y - radius >= 0 && center.x + radius < size.width &&
           center.y + radius < size.height;
}

void BallDetector::ResetBestCandidate() { best_candidate_ = DetectionCandidate{}; }

void BallDetector::UpdateCandidate(const cv::Point& center, const int radius, const double score)
{
    if (score <= best_candidate_.score)
    {
        return;
    }

    best_candidate_.found = true;
    best_candidate_.center = center;
    best_candidate_.radius = radius;
    best_candidate_.score = score;
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
    cv::circle(circle_mask, center, radius, cv::Scalar(255), cv::FILLED);

    cv::Mat white_overlap;
    cv::bitwise_and(mask, circle_mask, white_overlap);

    const double circle_area{static_cast<double>(cv::countNonZero(circle_mask))};
    if (circle_area <= 0.0)
    {
        return;
    }

    const double white_ratio{static_cast<double>(cv::countNonZero(white_overlap)) / circle_area};
    if (white_ratio < detector_config::kCircleMinWhiteRatio)
    {
        return;
    }

    const cv::Scalar mean_bgr{cv::mean(frame, circle_mask)};
    const double brightness{(mean_bgr[0] + mean_bgr[1] + mean_bgr[2]) / (3.0 * 255.0)};
    const double score{white_ratio * detector_config::kCircleWhiteRatioWeight +
                       brightness * detector_config::kCircleBrightnessWeight};

    UpdateCandidate(center, radius, score);
}

void BallDetector::CollectContourCandidate(const cv::Mat& frame,
                                           const std::vector<cv::Point>& contour)
{
    const double area{cv::contourArea(contour)};
    if (area < detector_config::kBallMinArea || area > detector_config::kBallMaxArea)
    {
        return;
    }

    const double perimeter{cv::arcLength(contour, true)};
    if (perimeter <= 0.0)
    {
        return;
    }

    const double circularity{4.0 * CV_PI * area / (perimeter * perimeter)};
    if (circularity < detector_config::kBallMinCircularity)
    {
        return;
    }

    cv::Point2f center_f;
    float radius_f;
    cv::minEnclosingCircle(contour, center_f, radius_f);
    if (radius_f < detector_config::kBallMinRadius || radius_f > detector_config::kBallMaxRadius)
    {
        return;
    }

    const cv::Rect box{cv::boundingRect(contour)};
    if (box.x <= 1 || box.y <= 1 || box.x + box.width >= frame.cols - 1 ||
        box.y + box.height >= frame.rows - 1)
    {
        return;
    }

    const double aspect_ratio{static_cast<double>(box.width) / static_cast<double>(box.height)};
    if (aspect_ratio < detector_config::kBallMinAspectRatio ||
        aspect_ratio > detector_config::kBallMaxAspectRatio)
    {
        return;
    }

    const double extent{area / static_cast<double>(box.area())};
    if (extent < detector_config::kBallMinExtent || extent > detector_config::kBallMaxExtent)
    {
        return;
    }

    const cv::Point center{static_cast<int>(std::round(center_f.x)),
                           static_cast<int>(std::round(center_f.y))};
    const int radius{static_cast<int>(std::round(radius_f))};
    UpdateCandidate(center, radius, circularity * extent);
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
    MaskUtils::WriteMaskIfVerbose(detector_config::kBackgroundDiffPath, diff);

    cv::Mat foreground_mask;
    cv::threshold(
      diff, foreground_mask, detector_config::kBackgroundDiffThreshold, 255, cv::THRESH_BINARY);

    cv::Mat foreground_kernel =
      MaskUtils::CreateKernel(detector_config::kForegroundKernelSize, cv::MORPH_ELLIPSE);
    cv::morphologyEx(foreground_mask, foreground_mask, cv::MORPH_OPEN, foreground_kernel);
    cv::morphologyEx(foreground_mask, foreground_mask, cv::MORPH_CLOSE, foreground_kernel);

    if (!playfield_mask.empty())
    {
        cv::bitwise_and(foreground_mask, playfield_mask, foreground_mask);
        cv::accumulateWeighted(
          gray, background_model_, detector_config::kBackgroundLearningRate, playfield_mask);
    }
    else
    {
        cv::accumulateWeighted(gray, background_model_, detector_config::kBackgroundLearningRate);
    }

    return foreground_mask;
}

void BallDetector::DrawDetection(cv::Mat& frame) const
{
    if (!best_candidate_.found)
    {
        return;
    }

    cv::circle(frame,
               best_candidate_.center,
               best_candidate_.radius,
               detector_config::kBallDrawColor,
               detector_config::kDrawThickness);
    MaskUtils::DrawLabel(frame, "Ball", best_candidate_.center, detector_config::kBallDrawColor);
}

cv::Mat BallDetector::Detect(cv::Mat& frame)
{
    cv::Mat mask{
      MaskUtils::BuildHsvMask(frame, detector_config::kLowerWhite, detector_config::kUpperWhite)};
    cv::Mat ball_kernel{
      MaskUtils::CreateKernel(detector_config::kBallKernelSize, cv::MORPH_ELLIPSE)};
    cv::morphologyEx(mask, mask, cv::MORPH_OPEN, ball_kernel);
    cv::morphologyEx(mask, mask, cv::MORPH_CLOSE, ball_kernel);

    std::vector<cv::Point> playfield_polygon;
    cv::Mat playfield_mask;
    if (playfield_detector_.Detect(frame, playfield_polygon, playfield_mask))
    {
        cv::polylines(frame,
                      playfield_polygon,
                      true,
                      detector_config::kPlayfieldDrawColor,
                      detector_config::kDrawThickness);
        MaskUtils::WriteMaskIfVerbose(detector_config::kFieldMaskPath, playfield_mask);
        cv::bitwise_and(mask, playfield_mask, mask);
    }

    cv::Mat color_mask{mask.clone()};

    cv::Mat gray;
    cv::cvtColor(frame, gray, cv::COLOR_BGR2GRAY);
    cv::GaussianBlur(gray, gray, cv::Size(9, 9), 2.0, 2.0);
    if (!playfield_mask.empty())
    {
        cv::bitwise_and(gray, playfield_mask, gray);
    }

    cv::Mat foreground_mask{BuildForegroundMask(gray, playfield_mask)};
    if (!foreground_mask.empty())
    {
        MaskUtils::WriteMaskIfVerbose(detector_config::kForegroundMaskPath, foreground_mask);

        cv::Mat motion_refined_mask;
        cv::bitwise_and(color_mask, foreground_mask, motion_refined_mask);

        if (cv::countNonZero(motion_refined_mask) >= detector_config::kForegroundMinPixels)
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

    MaskUtils::WriteMaskIfVerbose(detector_config::kBallMaskPath, mask);

    ResetBestCandidate();

    std::vector<cv::Vec3f> circles;
    cv::HoughCircles(gray,
                     circles,
                     cv::HOUGH_GRADIENT,
                     detector_config::kHoughDp,
                     detector_config::kHoughMinDist,
                     detector_config::kHoughParam1,
                     detector_config::kHoughParam2,
                     detector_config::kHoughMinRadius,
                     detector_config::kHoughMaxRadius);

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

    DrawDetection(frame);
    return frame;
}

cv::Mat detect_ball(cv::Mat& frame)
{
    static BallDetector detector;
    return detector.Detect(frame);
}
