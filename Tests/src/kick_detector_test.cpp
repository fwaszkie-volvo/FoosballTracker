#include "kick_detector.hpp"

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
#include "processing_config.hpp"
#include "utils.hpp"

using namespace kick_detector;

class KickDetectorTest : public ::testing::Test
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

TEST_F(KickDetectorTest, VideoReadAndAnnotatedWrite)
{
    const std::string input_file{test_utils::TestFilePath("test_goal_scored.mp4")};
    int processed_frame_count = 0;
    constexpr double kDefaultFps{30.0};
    constexpr double kDefaultShortVideoDurationSeconds{10.0};
    const int max_frames =
      static_cast<int>(std::ceil(kDefaultFps * kDefaultShortVideoDurationSeconds));
    int frame_count = 0;
    KickDetector kick_detector{};

    processor_.ProcessFrames(input_file,
                             [&](cv::Mat& frame) -> void
                             {
                                 if (frame_count >= max_frames)
                                 {
                                     return;
                                 }

                                 spdlog::debug("Processing frame {}", processed_frame_count);

                                 kick_detector.Detect(frame);
                                 kick_detector.Draw(frame);

                                 ++processed_frame_count;
                                 ++frame_count;
                             });

    EXPECT_TRUE(kick_detector.detected_once_);
    EXPECT_GT(processed_frame_count, 0) << "Input video had no readable frames: " << input_file;
}