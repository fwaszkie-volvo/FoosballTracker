#include <gtest/gtest.h>
#include <spdlog/spdlog.h>

#include <cmath>
#include <opencv2/opencv.hpp>
#include <string>

#include "detector.hpp"
#include "detector_types.hpp"
#include "frame_processor.hpp"
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
      .input_source = test_utils::TestFilePath("test_video.mp4"),
      .output_path = detector_types::kOutputVideoPath,
    };

    int processed_frame_count = 0;
    constexpr double kDefaultFps{30.0};
    constexpr double kDefaultShortVideoDurationSeconds{75.0};
    const int max_frames =
      static_cast<int>(std::ceil(kDefaultFps * kDefaultShortVideoDurationSeconds));
    int frame_count = 0;

    const auto result =
      processor_.ProcessFrames(process_target,
                               [&](cv::Mat& frame) -> void
                               {
                                   if (frame_count >= max_frames)
                                   {
                                       return;
                                   }

                                   spdlog::debug("Processing frame {}", processed_frame_count);
                                   detect_ball(frame);
                                   ++processed_frame_count;
                                   ++frame_count;
                               });

    ASSERT_TRUE(result.has_value()) << "Failed to process video";
    EXPECT_GT(processed_frame_count, 0)
      << "Input video had no readable frames: " << detector_types::kInputVideoPath;
}
