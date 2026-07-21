#ifndef FOOSBALL_TRACKER_DETECTOR_CONFIG_HPP_
#define FOOSBALL_TRACKER_DETECTOR_CONFIG_HPP_

#include <opencv2/opencv.hpp>

namespace detector_config
{
const cv::Scalar kLowerGreen(17, 22, 22);
const cv::Scalar kUpperGreen(100, 255, 255);

const cv::Scalar kLowerWhite(0, 0, 170);
const cv::Scalar kUpperWhite(180, 90, 255);

const cv::Scalar kBlueDrawColor(0, 255, 0);
const cv::Scalar kRedDrawColor(0, 0, 255);
const cv::Scalar kBallDrawColor(0, 255, 255);
const cv::Scalar kPlayfieldDrawColor(255, 255, 0);

constexpr int kPlayfieldKernelSize = 15;
constexpr int kBallKernelSize = 7;
constexpr int kPlayfieldDilateIterations = 1;
constexpr int kGreenDominanceThreshold = 5;

constexpr double kPlayfieldApproxStart = 0.01;
constexpr double kPlayfieldApproxEnd = 0.08;
constexpr double kPlayfieldApproxStep = 0.005;
constexpr int kPlayfieldTargetVertices = 8;

constexpr int kBallMinArea = 80;
constexpr int kBallMaxArea = 5000;
constexpr double kBallMinCircularity = 0.75;
constexpr float kBallMinRadius = 5.0f;
constexpr float kBallMaxRadius = 45.0f;
constexpr double kBallMinAspectRatio = 0.75;
constexpr double kBallMaxAspectRatio = 1.33;
constexpr double kBallMinExtent = 0.55;
constexpr double kBallMaxExtent = 0.95;

constexpr double kCircleWhiteRatioWeight = 0.7;
constexpr double kCircleBrightnessWeight = 0.3;
constexpr double kCircleMinWhiteRatio = 0.35;

constexpr double kHoughDp = 1.2;
constexpr double kHoughMinDist = 20.0;
constexpr double kHoughParam1 = 120.0;
constexpr double kHoughParam2 = 12.0;
constexpr int kHoughMinRadius = 5;
constexpr int kHoughMaxRadius = 45;

constexpr int kDrawThickness = 2;
constexpr double kLabelScale = 0.6;
constexpr int kLabelYOffset = 10;
constexpr bool kVerbose = false;

// Temporary (hopefully) - works with run script
constexpr const char* kInputImagePath = "Tests/test_files/ball_unobscured.jpg";
constexpr const char* kOutputImagePath = "Tests/test_outputs/output.jpg";
constexpr const char* kFieldMaskPath = "Tests/test_outputs/field_mask.jpg";
constexpr const char* kBallMaskPath = "Tests/test_outputs/ball_mask.jpg";
}  // namespace detector_config

#endif  // FOOSBALL_TRACKER_DETECTOR_CONFIG_HPP_
