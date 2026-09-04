#include "playfield_detector.hpp"

#include <gtest/gtest.h>

#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>

namespace
{
const cv::Size kFrameSize{320, 240};
constexpr int kPlayfieldMargin{30};
constexpr int kShadowHalfWidth{12};
constexpr int kGoalWidth{25};
constexpr int kGoalHalfHeight{40};
constexpr int kTaperedEndHalfHeight{50};
constexpr int kTaperTransitionInset{70};
constexpr double kMinimumPlayfieldCoverage{0.9};
const cv::Scalar kBackgroundColor{3, 3, 3};
const cv::Scalar kNormalLightGreen{20, 100, 20};
const cv::Scalar kLowLightGreen{3, 8, 3};
const cv::Scalar kLowLightWarmGreen{3, 8, 7};

cv::Mat MakePlayfieldFrame(const cv::Scalar& playfield_color)
{
    cv::Mat frame{kFrameSize, CV_8UC3, kBackgroundColor};
    const cv::Point top_left{kPlayfieldMargin, kPlayfieldMargin};
    const cv::Point bottom_right{kFrameSize.width - kPlayfieldMargin,
                                 kFrameSize.height - kPlayfieldMargin};
    cv::rectangle(frame, top_left, bottom_right, playfield_color, cv::FILLED);
    return frame;
}
}  // namespace

TEST(PlayfieldDetectorTest, DetectsGreenPlayfieldAtDifferentBrightnessLevels)
{
    for (const auto& playfield_color : {kNormalLightGreen, kLowLightGreen, kLowLightWarmGreen})
    {
        PlayfieldDetector detector;
        detector.Detect(MakePlayfieldFrame(playfield_color));

        EXPECT_TRUE(detector.HasDetection());
        EXPECT_FALSE(detector.GetPolygon().empty());
    }
}

TEST(PlayfieldDetectorTest, CoversPlayfieldSplitByUnevenLighting)
{
    cv::Mat frame{MakePlayfieldFrame(kNormalLightGreen)};
    const int center_x{kFrameSize.width / 2};
    cv::rectangle(frame,
                  cv::Point{center_x - kShadowHalfWidth, kPlayfieldMargin},
                  cv::Point{center_x + kShadowHalfWidth, kFrameSize.height - kPlayfieldMargin},
                  kBackgroundColor,
                  cv::FILLED);

    PlayfieldDetector detector;
    detector.Detect(frame);

    const int playfield_area{(kFrameSize.width - (2 * kPlayfieldMargin)) *
                             (kFrameSize.height - (2 * kPlayfieldMargin))};
    ASSERT_TRUE(detector.HasDetection());
    EXPECT_GE(cv::countNonZero(detector.GetMask()),
              static_cast<int>(kMinimumPlayfieldCoverage * playfield_area));
}

TEST(PlayfieldDetectorTest, ExcludesGreenSideGoalFromPlayfield)
{
    PlayfieldDetector baseline_detector;
    baseline_detector.Detect(MakePlayfieldFrame(kNormalLightGreen));
    const cv::Rect baseline_bounds{cv::boundingRect(baseline_detector.GetMask())};

    cv::Mat frame{MakePlayfieldFrame(kNormalLightGreen)};
    const int center_y{kFrameSize.height / 2};
    cv::rectangle(frame,
                  cv::Point{kPlayfieldMargin - kGoalWidth, center_y - kGoalHalfHeight},
                  cv::Point{kPlayfieldMargin - 1, center_y + kGoalHalfHeight},
                  kLowLightGreen,
                  cv::FILLED);

    PlayfieldDetector detector;
    detector.Detect(frame);

    ASSERT_TRUE(detector.HasDetection());
    EXPECT_EQ(cv::boundingRect(detector.GetMask()), baseline_bounds);
}

TEST(PlayfieldDetectorTest, KeepsTaperedPlayfieldEndsBehindGoalkeepers)
{
        cv::Mat frame{kFrameSize, CV_8UC3, kBackgroundColor};
        const int center_y{kFrameSize.height / 2};
        const Contour tapered_playfield{
            cv::Point{kPlayfieldMargin, center_y - kTaperedEndHalfHeight},
            cv::Point{kPlayfieldMargin + kTaperTransitionInset, kPlayfieldMargin},
            cv::Point{kFrameSize.width - kPlayfieldMargin - kTaperTransitionInset, kPlayfieldMargin},
    cv::Point{kFrameSize.width - kPlayfieldMargin, center_y - kTaperedEndHalfHeight},
    cv::Point{kFrameSize.width - kPlayfieldMargin, center_y + kTaperedEndHalfHeight},
            cv::Point{kFrameSize.width - kPlayfieldMargin - kTaperTransitionInset,
                                kFrameSize.height - kPlayfieldMargin},
            cv::Point{kPlayfieldMargin + kTaperTransitionInset,
                                kFrameSize.height - kPlayfieldMargin},
            cv::Point{kPlayfieldMargin, center_y + kTaperedEndHalfHeight}};
        cv::fillConvexPoly(frame, tapered_playfield, kNormalLightGreen);

        PlayfieldDetector detector;
        detector.Detect(frame);

        ASSERT_TRUE(detector.HasDetection());
        const cv::Rect bounds{cv::boundingRect(detector.GetMask())};
        EXPECT_LE(bounds.x, kPlayfieldMargin);
        EXPECT_GE(bounds.x + bounds.width, kFrameSize.width - kPlayfieldMargin);
}
