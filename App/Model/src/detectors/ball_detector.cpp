#include "ball_detector.hpp"

#include <bits/std_abs.h>
#include <opencv2/core/hal/interface.h>

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <iomanip>
#include <opencv2/core.hpp>
#include <opencv2/core/fast_math.hpp>
#include <opencv2/core/mat.inl.hpp>
#include <opencv2/core/matx.hpp>
#include <opencv2/core/traits.hpp>
#include <opencv2/imgproc.hpp>
#include <sstream>
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

void BallDetector::UpdateCandidate(const cv::Point& center, const double score)
{
    if (score <= best_candidate_.score)
    {
        return;
    }

    best_candidate_.found  = true;
    best_candidate_.center = center;
    best_candidate_.score  = score;
}

cv::Point2f BallDetector::ComputeWindowSpeed() const
{
    if (position_history_.size() < 2)
    {
        return cv::Point2f{};
    }

    const std::size_t latest_index{position_history_.size() - 1};
    const std::size_t frame_span{std::min<std::size_t>(
      static_cast<std::size_t>(detector_types::kBallSpeedWindowFrames), latest_index)};
    const std::size_t reference_index{latest_index - frame_span};
    return (position_history_[latest_index] - position_history_[reference_index]) /
           static_cast<float>(frame_span);
}

void BallDetector::UpdateMeasurement(const cv::Point& center)
{
    BallMeasurement updated_measurement{};
    updated_measurement.found = true;
    const cv::Point2f raw_position{static_cast<float>(center.x), static_cast<float>(center.y)};
    if (has_previous_position_)
    {
        const cv::Point2f frame_delta{raw_position - previous_position_};
        const float frame_displacement{static_cast<float>(cv::norm(frame_delta))};
        float smoothing_factor{detector_types::kBallPositionSmoothingFactor};
        if (frame_displacement >= detector_types::kBallPositionFastMotionThreshold)
        {
            smoothing_factor = detector_types::kBallPositionFastSmoothingFactor;
        }

        updated_measurement.position = previous_position_ + frame_delta * smoothing_factor;
    }
    else
    {
        updated_measurement.position = raw_position;
    }

    position_history_.push_back(updated_measurement.position);
    const std::size_t max_history_size{
      static_cast<std::size_t>(detector_types::kBallSpeedWindowFrames + 1)};
    if (position_history_.size() > max_history_size)
    {
        position_history_.pop_front();
    }

    updated_measurement.speed = ComputeWindowSpeed();

    if (cv::norm(updated_measurement.speed) < detector_types::kBallVelocityDeadbandPixels)
    {
        updated_measurement.speed = cv::Point2f{};
    }

    updated_measurement.size = detector_types::kBallFixedRadius;

    previous_position_     = updated_measurement.position;
    has_previous_position_ = true;
    measurement_           = updated_measurement;
}

cv::Mat BallDetector::BuildColorMask(const cv::Mat& frame) const
{
    cv::Mat mask{
      mask_utils::build_hsv_mask(frame, detector_types::kLowerWhite, detector_types::kUpperWhite)};
    cv::Mat ball_kernel{
      mask_utils::create_kernel(detector_types::kBallKernelSize, cv::MORPH_ELLIPSE)};
    cv::morphologyEx(mask, mask, cv::MORPH_OPEN, ball_kernel);
    cv::morphologyEx(mask, mask, cv::MORPH_CLOSE, ball_kernel);
    return mask;
}

cv::Mat BallDetector::BuildGrayFrame(const cv::Mat& frame, const cv::Mat& playfield_mask) const
{
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

    return gray;
}

cv::Mat BallDetector::BuildDetectionMask(const cv::Mat& color_mask,
                                         const cv::Mat& gray,
                                         const cv::Mat& playfield_mask,
                                         bool& has_reliable_foreground)
{
    cv::Mat mask{color_mask.clone()};
    cv::Mat foreground_mask{BuildForegroundMask(gray, playfield_mask)};
    has_reliable_foreground = false;
    if (!foreground_mask.empty())
    {
        mask_utils::write_mask_if_verbose(detector_types::kForegroundMaskPath, foreground_mask);

        cv::bitwise_and(color_mask, foreground_mask, mask);

        if (cv::countNonZero(mask) < detector_types::kForegroundMinPixels)
        {
            mask = color_mask;
        }
        else
        {
            has_reliable_foreground = true;
        }
    }

    return mask;
}

void BallDetector::ScoreHoughCandidates(const cv::Mat& frame,
                                        const cv::Mat& gray,
                                        const cv::Mat& mask)
{
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
        const cv::Point center{static_cast<int>(std::round(circle[0])),
                               static_cast<int>(std::round(circle[1]))};
        const int radius{static_cast<int>(std::round(circle[2]))};
        CollectHoughCandidate(frame, mask, center, radius);
    }
}

void BallDetector::UpdateTrackingState()
{
    if (best_candidate_.found)
    {
        missed_detection_frames_ = 0;
        UpdateMeasurement(best_candidate_.center);
        return;
    }

    ++missed_detection_frames_;

    if (has_previous_position_ &&
        missed_detection_frames_ < detector_types::kBallTrackingResetMissFrames)
    {
        BallMeasurement predicted_measurement{};
        predicted_measurement.found = false;
        predicted_measurement.speed = ComputeWindowSpeed();
        if (cv::norm(predicted_measurement.speed) < detector_types::kBallVelocityDeadbandPixels)
        {
            predicted_measurement.speed = cv::Point2f{};
        }
        predicted_measurement.position = previous_position_ + predicted_measurement.speed;
        predicted_measurement.size     = detector_types::kBallFixedRadius;

        previous_position_ = predicted_measurement.position;
        measurement_       = predicted_measurement;
    }

    if (missed_detection_frames_ >= detector_types::kBallTrackingResetMissFrames)
    {
        previous_position_     = cv::Point2f{};
        has_previous_position_ = false;
        position_history_.clear();
    }
}

void BallDetector::ResetTrackingState()
{
    ResetBestCandidate();
    previous_position_     = cv::Point2f{};
    has_previous_position_ = false;
    position_history_.clear();
    missed_detection_frames_ = 0;
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

    const cv::Rect circle_bounds{center.x - radius, center.y - radius, 2 * radius, 2 * radius};
    const cv::Point local_center{radius, radius};

    cv::Mat circle_mask{cv::Mat::zeros(circle_bounds.size(), CV_8UC1)};
    cv::circle(
      circle_mask, local_center, radius, cv::Scalar(detector_types::kWhitePixelValue), cv::FILLED);

    cv::Mat white_overlap;
    cv::bitwise_and(mask(circle_bounds), circle_mask, white_overlap);

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

    const cv::Scalar mean_bgr{cv::mean(frame(circle_bounds), circle_mask)};
    const double brightness{
      (mean_bgr[0] + mean_bgr[1] + mean_bgr[2]) /
      (detector_types::kColorChannels * detector_types::kBrightnessNormalization)};
    const double score{white_ratio * detector_types::kCircleWhiteRatioWeight +
                       brightness * detector_types::kCircleBrightnessWeight};

    UpdateCandidate(center, score);
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

double BallDetector::ComputeMetersPerPixel() const
{
    const auto& playfield_polygon = playfield_detector_.GetPolygon();

    const auto playfield_area_pixels = std::abs(cv::contourArea(playfield_polygon));

    const auto playfield_area_meters =
      detector_types::kPlayfieldWidthMeters * detector_types::kPlayfieldHeightMeters;

    return std::sqrt(playfield_area_meters / playfield_area_pixels);
}

double BallDetector::ComputeBallSpeedMetersPerSecond() const
{
    const auto meters_per_pixel = ComputeMetersPerPixel();

    const auto speed_pixels_per_frame = cv::norm(measurement_.speed);

    return speed_pixels_per_frame * detector_types::kDefaultProcessingFps * meters_per_pixel;
}

void BallDetector::Draw(cv::Mat& frame) const
{
    playfield_detector_.Draw(frame);

    if (!measurement_.found)
    {
        return;
    }

    const cv::Point center{cvRound(measurement_.position.x), cvRound(measurement_.position.y)};
    const int radius{cvRound(measurement_.size)};
    const cv::Point velocity_end{
      center + cv::Point{cvRound(measurement_.speed.x * detector_types::kVelocityArrowScale),
                         cvRound(measurement_.speed.y * detector_types::kVelocityArrowScale)}};

    cv::circle(
      frame, center, radius, detector_types::kBallDrawColor, detector_types::kDrawThickness);
    mask_utils::draw_label(frame, "Ball", center, detector_types::kBallDrawColor);
    cv::arrowedLine(frame,
                    center,
                    velocity_end,
                    detector_types::kVelocityArrowColor,
                    detector_types::kVelocityArrowThickness,
                    cv::LINE_AA,
                    0,
                    detector_types::kVelocityArrowTipLength);

    const auto speed_meters_per_second = ComputeBallSpeedMetersPerSecond();

    const auto speed_kilometers_per_hour =
      speed_meters_per_second * detector_types::kMetersPerSecondToKilometersPerHour;

    std::ostringstream speed_label;
    speed_label << std::fixed << std::setprecision(2) << speed_meters_per_second << " m/s ("
                << speed_kilometers_per_hour << " km/h)";

    const cv::Point speed_label_position{center.x, center.y + 2 * detector_types::kLabelYOffset};
    mask_utils::draw_label(
      frame, speed_label.str(), speed_label_position, detector_types::kVelocityArrowColor);
}

void BallDetector::Detect(const cv::Mat& frame)
{
    measurement_ = BallMeasurement{};

    playfield_detector_.Detect(frame);
    const cv::Mat& playfield_mask{playfield_detector_.GetMask()};

    cv::Rect detection_roi{0, 0, frame.cols, frame.rows};
    if (playfield_detector_.HasDetection())
    {
        const cv::Rect playfield_bounds{cv::boundingRect(playfield_detector_.GetPolygon())};
        const cv::Rect clamped_bounds{playfield_bounds & cv::Rect{0, 0, frame.cols, frame.rows}};
        if (clamped_bounds.width > 0 && clamped_bounds.height > 0)
        {
            detection_roi = clamped_bounds;
        }
    }

    const cv::Mat frame_roi{frame(detection_roi)};
    const cv::Mat playfield_mask_roi{playfield_mask.empty() ? cv::Mat{}
                                                            : playfield_mask(detection_roi)};

    cv::Mat color_mask{BuildColorMask(frame_roi)};
    if (!playfield_mask_roi.empty())
    {
        mask_utils::write_mask_if_verbose(detector_types::kFieldMaskPath, playfield_mask_roi);
        cv::bitwise_and(color_mask, playfield_mask_roi, color_mask);
    }

    cv::Mat gray{BuildGrayFrame(frame_roi, playfield_mask_roi)};
    mask_utils::write_mask_if_verbose(detector_types::kGrayMaskPath, gray);
    bool has_reliable_foreground{false};
    cv::Mat mask{BuildDetectionMask(color_mask, gray, playfield_mask_roi, has_reliable_foreground)};
    mask_utils::write_mask_if_verbose(detector_types::kBallMaskPath, mask);

    if (has_reliable_foreground &&
        foreground_confirmed_frames_ < detector_types::kBallStartupForegroundWarmupFrames)
    {
        ++foreground_confirmed_frames_;
    }

    if (foreground_confirmed_frames_ < detector_types::kBallStartupForegroundWarmupFrames)
    {
        ResetTrackingState();
        return;
    }

    ResetBestCandidate();
    ScoreHoughCandidates(frame_roi, gray, mask);
    if (best_candidate_.found)
    {
        best_candidate_.center += detection_roi.tl();
    }
    UpdateTrackingState();
}

cv::Mat detect_ball(cv::Mat& frame)
{
    static BallDetector detector;
    detector.Detect(frame);
    detector.Draw(frame);
    return frame;
}
