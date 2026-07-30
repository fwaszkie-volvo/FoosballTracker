#include "ball_detector.hpp"

#include <gtest/gtest.h>
#include <spdlog/spdlog.h>

#include <cmath>
#include <iomanip>
#include <opencv2/opencv.hpp>
#include <sstream>
#include <string>

#include "detector.hpp"
#include "detector_types.hpp"
#include "frame_processor.hpp"
#include "goal_detector.hpp"
#include "processing_config.hpp"
#include "utils.hpp"

class BallDetectorTest : public ::testing::Test
{
  protected:
    void SetUp() override
    {
        frame_ = cv::imread(detector_types::kInputImagePath, cv::IMREAD_COLOR);
        processor_.SetReaderType(ReaderType::kOnline);
    }

    cv::Mat frame_;
    FrameProcessor processor_{};
};

TEST_F(BallDetectorTest, ImageLoads)
{
    ASSERT_FALSE(frame_.empty()) << "Failed to load input image: "
                                 << detector_types::kInputImagePath;
}

TEST_F(BallDetectorTest, BallDetectionRuns)
{
    ASSERT_FALSE(frame_.empty());

    detect_ball(frame_);

    EXPECT_FALSE(frame_.empty());
    bool success{cv::imwrite(detector_types::kOutputImagePath, frame_)};
    EXPECT_TRUE(success) << "Failed to write output image: " << detector_types::kOutputImagePath;
}

TEST_F(BallDetectorTest, VideoReadAndAnnotatedWrite)
{
    config::ProcessingTarget process_target{
      .input_source = test_utils::TestFilePath("test_video_trimmed.mp4"),
      .output_path = test_utils::TestOutputPath("output_annotated_video.mp4"),
    };

    int processed_frame_count = 0;
    constexpr double kDefaultFps{30.0};
    constexpr double kDefaultShortVideoDurationSeconds{10.0};
    const int max_frames =
      static_cast<int>(std::ceil(kDefaultFps * kDefaultShortVideoDurationSeconds));
    int frame_count = 0;
    BallDetector ball_detector{};

    const auto result =
      processor_.ProcessFrames(process_target,
                               [&](cv::Mat& frame) -> void
                               {
                                   if (frame_count >= max_frames)
                                   {
                                       return;
                                   }

                                   spdlog::debug("Processing frame {}", processed_frame_count);
                                   ball_detector.Detect(frame);

                                   ball_detector.Draw(frame);

                                   ++processed_frame_count;
                                   ++frame_count;
                               });

    ASSERT_TRUE(result.has_value()) << "Failed to process video";
    EXPECT_GT(processed_frame_count, 0)
      << "Input video had no readable frames: " << process_target.input_source;
}

TEST_F(BallDetectorTest, VideoReadAndAnnotatedWriteWithGoalDetection)
{
    config::ProcessingTarget process_target{
      .input_source = test_utils::TestFilePath("test_goal_scored.mp4"),
      .output_path = test_utils::TestOutputPath("output_annotated_goal_video.mp4"),
    };

    int processed_frame_count = 0;
    constexpr double kDefaultFps{30.0};
    constexpr double kDefaultShortVideoDurationSeconds{10.0};
    const int max_frames =
      static_cast<int>(std::ceil(kDefaultFps * kDefaultShortVideoDurationSeconds));
    int frame_count = 0;
    BallDetector ball_detector{};
    GoalDetector goal_detector{};

    const auto result =
      processor_.ProcessFrames(process_target,
                               [&](cv::Mat& frame) -> void
                               {
                                   if (frame_count >= max_frames)
                                   {
                                       return;
                                   }

                                   spdlog::debug("Processing frame {}", processed_frame_count);
                                   ball_detector.Detect(frame);
                                   goal_detector.Detect(frame);

                                   ball_detector.Draw(frame);
                                   goal_detector.Draw(frame);

                                   ++processed_frame_count;
                                   ++frame_count;
                               });

    ASSERT_TRUE(result.has_value()) << "Failed to process video";
    EXPECT_GT(processed_frame_count, 0)
      << "Input video had no readable frames: " << process_target.input_source;
}
