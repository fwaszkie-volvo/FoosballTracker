#ifndef FOOSBALL_TRACKER_APP_MODEL_INCLUDE_PRIVATE_UTILS_DETECTOR_TYPES_HPP_
#define FOOSBALL_TRACKER_APP_MODEL_INCLUDE_PRIVATE_UTILS_DETECTOR_TYPES_HPP_

#include <opencv2/opencv.hpp>
#include <string>

#include "mask_utils.hpp"

using Contour = std::vector<cv::Point>;

namespace detector_types
{
const cv::Scalar kLowerGreen(17, 22, 22);
const cv::Scalar kUpperGreen(100, 255, 255);

const cv::Scalar kLowerWhite(0, 0, 170);
const cv::Scalar kUpperWhite(180, 90, 255);

const cv::Scalar kBallDrawColor(0, 255, 255);
const cv::Scalar kPlayfieldDrawColor(255, 255, 0);
const cv::Scalar kPlayersDrawRedColor(0, 0, 255);
const cv::Scalar kPlayersDrawBlueColor(255, 0, 0);

const cv::Scalar kPlayersLowerBlue(100, 150, 50);
const cv::Scalar kPlayersUpperBlue(140, 255, 255);

const cv::Scalar kPlayersLowerRed1(0, 120, 70);
const cv::Scalar kPlayersUpperRed1(10, 255, 255);
const cv::Scalar kPlayersLowerRed2(170, 120, 70);
const cv::Scalar kPlayersUpperRed2(180, 255, 255);

constexpr int kPlayfieldKernelSize{15};
constexpr int kBallKernelSize{7};
constexpr int kPlayersKernelSize{5};
constexpr int kPlayfieldDilateIterations{1};
constexpr int kGreenDominanceThreshold{5};

constexpr double kPlayfieldApproxStart{0.01};
constexpr double kPlayfieldApproxEnd{0.08};
constexpr double kPlayfieldApproxStep{0.005};
constexpr int kPlayfieldTargetVertices{8};

constexpr int kPlayersMinArea{500};
constexpr float kBallFixedRadius{19.0f};

constexpr double kCircleWhiteRatioWeight{0.7};
constexpr double kCircleBrightnessWeight{0.3};
constexpr double kCircleMinWhiteRatio{0.35};

constexpr double kHoughDp{1.2};
constexpr double kHoughMinDist{20.0};
constexpr double kHoughParam1{120.0};
constexpr double kHoughParam2{12.0};
constexpr int kHoughMinRadius{19};
constexpr int kHoughMaxRadius{21};

constexpr double kBackgroundLearningRate{0.02};
constexpr int kBackgroundDiffThreshold{24};
constexpr int kForegroundKernelSize{5};
constexpr int kForegroundMinPixels{30};
constexpr int kBallStartupForegroundWarmupFrames{3};

constexpr double kBallPositionSmoothingFactor{0.35};
constexpr double kBallPositionFastSmoothingFactor{0.75};
constexpr double kBallPositionFastMotionThreshold{6.0};
constexpr int kBallSpeedWindowFrames{5};
constexpr double kBallVelocityDeadbandPixels{0.5};
constexpr int kBallTrackingResetMissFrames{5};

constexpr int kWhitePixelValue{255};
constexpr int kColorChannels{3};
constexpr double kBrightnessNormalization{255.0};
const cv::Size kGaussianBlurKernelSize{9, 9};
constexpr double kGaussianBlurSigma{2.0};

constexpr int kDrawThickness{2};
constexpr double kLabelScale{0.6};
constexpr int kLabelYOffset{10};
constexpr bool kVerbose{false};

inline const std::string kTestFilesDir{"Tests/test_files/"};
inline const std::string kTestOutputsDir{"Tests/test_outputs/"};

inline const std::string kInputImagePath{kTestFilesDir + "ball_unobscured.jpg"};
inline const std::string kInputVideoPath{kTestFilesDir + "test_video.mp4"};
inline const std::string kOutputImagePath{kTestOutputsDir + "output.jpg"};
inline const std::string kOutputVideoPath{kTestOutputsDir + "output.mp4"};
inline const std::string kFieldMaskPath{kTestOutputsDir + "field_mask.jpg"};
inline const std::string kBallMaskPath{kTestOutputsDir + "ball_mask.jpg"};
inline const std::string kGrayMaskPath{kTestOutputsDir + "gray_mask.jpg"};
inline const std::string kForegroundMaskPath{kTestOutputsDir + "foreground_mask.jpg"};
inline const std::string kBackgroundDiffPath{kTestOutputsDir + "background_diff.jpg"};

const cv::Scalar kVelocityArrowColor{0, 0, 255};
constexpr double kVelocityArrowScale{8.0};
constexpr int kVelocityArrowThickness{2};
constexpr double kVelocityArrowTipLength{0.25};

struct BlueColorRange
{
    static cv::Mat CreateMask(const cv::Mat& frame)
    {
        return mask_utils::build_hsv_mask(frame, kPlayersLowerBlue, kPlayersUpperBlue);
    }
};

struct RedColorRange
{
    static cv::Mat CreateMask(const cv::Mat& frame)
    {
        cv::Mat mask1{mask_utils::build_hsv_mask(frame, kPlayersLowerRed1, kPlayersUpperRed1)};
        cv::Mat mask2{mask_utils::build_hsv_mask(frame, kPlayersLowerRed2, kPlayersUpperRed2)};
        cv::Mat mask;
        cv::bitwise_or(mask1, mask2, mask);
        return mask;
    }
};
}  // namespace detector_types

#endif  // FOOSBALL_TRACKER_APP_MODEL_INCLUDE_PRIVATE_UTILS_DETECTOR_TYPES_HPP_
