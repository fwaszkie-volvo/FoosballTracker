#include "playfield_detector.hpp"

#include <gtest/gtest.h>

#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/videoio.hpp>

#include "utils.hpp"

namespace
{
const cv::Size kFrameSize{320, 240};
constexpr int kPlayfieldMargin{30};
constexpr int kShadowHalfWidth{12};
constexpr int kGoalWidth{25};
constexpr int kGoalHalfHeight{40};
constexpr int kTaperedEndHalfHeight{50};
constexpr int kTaperTransitionInset{70};
constexpr int kFalseRegionWidth{190};
constexpr int kFalseRegionHeight{150};
constexpr int kGoalRecessWidth{30};
constexpr int kGoalRecessHalfHeight{35};
constexpr double kMinimumPlayfieldCoverage{0.9};
constexpr double kGoal1MinFrameWidthRatio{0.75};
constexpr double kGoal1MinFrameHeightRatio{0.75};
constexpr double kGoal1MinFrameAreaRatio{0.52};
constexpr double kGoal1GoalOpeningTimestampMilliseconds{36000.0};
const cv::Point kGoal1RightGoalOpeningPoint{1785, 555};
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
        const cv::Rect expected_bounds{kPlayfieldMargin,
                                       kPlayfieldMargin,
                                       kFrameSize.width - (2 * kPlayfieldMargin) + 1,
                                       kFrameSize.height - (2 * kPlayfieldMargin) + 1};
        EXPECT_EQ(cv::boundingRect(detector.GetMask()), expected_bounds);
    }
}

TEST(PlayfieldDetectorTest, DetectsFullPlayfieldInGoal1Video)
{
    cv::VideoCapture video{test_utils::TestFilePath("test_video_goal_1.mp4")};
    cv::Mat frame;
    ASSERT_TRUE(video.read(frame));

    PlayfieldDetector detector;
    detector.Detect(frame);

    ASSERT_TRUE(detector.HasDetection());
    const cv::Rect bounds{cv::boundingRect(detector.GetMask())};
    const double frame_area{static_cast<double>(frame.rows) * static_cast<double>(frame.cols)};
    EXPECT_GE(bounds.width, frame.cols * kGoal1MinFrameWidthRatio);
    EXPECT_GE(bounds.height, frame.rows * kGoal1MinFrameHeightRatio);
    EXPECT_GE(cv::countNonZero(detector.GetMask()), frame_area * kGoal1MinFrameAreaRatio);
    EXPECT_GE(detector.GetPolygon().size(), 8U);
}

TEST(PlayfieldDetectorTest, ExcludesGoalOpeningInGoal1Video)
{
    cv::VideoCapture video{test_utils::TestFilePath("test_video_goal_1.mp4")};
    video.set(cv::CAP_PROP_POS_MSEC, kGoal1GoalOpeningTimestampMilliseconds);
    cv::Mat frame;
    ASSERT_TRUE(video.read(frame));

    PlayfieldDetector detector;
    detector.Detect(frame);

    ASSERT_TRUE(detector.HasDetection());
    EXPECT_EQ(detector.GetMask().at<std::uint8_t>(kGoal1RightGoalOpeningPoint), 0);
}

TEST(PlayfieldDetectorTest, RejectsSmallGreenRegionInFrameCorner)
{
    cv::Mat frame{kFrameSize, CV_8UC3, kBackgroundColor};
    cv::rectangle(frame,
                  cv::Point{0, 0},
                  cv::Point{kFalseRegionWidth, kFalseRegionHeight},
                  kNormalLightGreen,
                  cv::FILLED);

    PlayfieldDetector detector;
    detector.Detect(frame);

    EXPECT_FALSE(detector.HasDetection());
    EXPECT_TRUE(detector.GetMask().empty());
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

TEST(PlayfieldDetectorTest, IncludesGreenPlayfieldEndsNearGoals)
{
    PlayfieldDetector baseline_detector;
    baseline_detector.Detect(MakePlayfieldFrame(kNormalLightGreen));

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
    EXPECT_LT(cv::boundingRect(detector.GetMask()).x,
              cv::boundingRect(baseline_detector.GetMask()).x);
}

TEST(PlayfieldDetectorTest, FillsPlayfieldBetweenFragmentsNearGoal)
{
    cv::Mat frame{MakePlayfieldFrame(kNormalLightGreen)};
    const int center_y{kFrameSize.height / 2};
    const cv::Rect goal_recess{kPlayfieldMargin,
                               center_y - kGoalRecessHalfHeight,
                               kGoalRecessWidth,
                               2 * kGoalRecessHalfHeight};
    cv::rectangle(frame, goal_recess, kBackgroundColor, cv::FILLED);

    PlayfieldDetector detector;
    detector.Detect(frame);

    ASSERT_TRUE(detector.HasDetection());
    EXPECT_EQ(detector.GetMask().at<std::uint8_t>(goal_recess.y + (goal_recess.height / 2),
                                                  goal_recess.x + (goal_recess.width / 2)),
              255);
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
      cv::Point{kPlayfieldMargin + kTaperTransitionInset, kFrameSize.height - kPlayfieldMargin},
      cv::Point{kPlayfieldMargin, center_y + kTaperedEndHalfHeight}};
    cv::fillConvexPoly(frame, tapered_playfield, kNormalLightGreen);

    PlayfieldDetector detector;
    detector.Detect(frame);

    ASSERT_TRUE(detector.HasDetection());
    const cv::Rect bounds{cv::boundingRect(detector.GetMask())};
    EXPECT_LE(bounds.x, kPlayfieldMargin);
    EXPECT_GE(bounds.x + bounds.width, kFrameSize.width - kPlayfieldMargin);
}
