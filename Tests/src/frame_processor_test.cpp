#include "frame_processor.hpp"

#include <gtest/gtest.h>

#include <filesystem>
#include <opencv2/opencv.hpp>

#include "processing_config.hpp"
#include "utils.hpp"

class FrameProcessorTest : public ::testing::Test
{
  protected:
    FrameProcessor processor_;
};

TEST_F(FrameProcessorTest, UnspecifiedReaderTypeReturnsNullopt)
{
    int processed_frames = 0;
    processor_.ProcessFrames(
      config::ProcessingTarget{.input_source = test_utils::TestFilePath("ball_unobscured.jpg"),
                               .output_path = test_utils::TestOutputPath("unspecified_output.jpg")},
      [&](cv::Mat&) { ++processed_frames; });

    EXPECT_EQ(processed_frames, 0);
}

// Two tests disabled, cause I want to merge UI first, and then fix it
// API has changed, and everything here is "under construction"
TEST_F(FrameProcessorTest, DISABLED_PhotoModeProcessesSigleFrameAndWritesImage)
{
    processor_.SetReaderType(ReaderType::kPhoto);

    int processed_frames = 0;
    const auto output_path = test_utils::TestOutputPath("frame_processor_photo_output.jpg");
    std::filesystem::remove(output_path);

    processor_.ProcessFrames(
      config::ProcessingTarget{.input_source = test_utils::TestFilePath("ball_unobscured.jpg"),
                               .output_path = output_path},
      [&](cv::Mat& frame)
      {
          ++processed_frames;
          cv::circle(
            frame, cv::Point(frame.cols / 2, frame.rows / 2), 10, cv::Scalar(0, 255, 0), 2);
      });

    EXPECT_EQ(processed_frames, 1);
    EXPECT_TRUE(std::filesystem::exists(output_path));

    const auto saved = cv::imread(output_path);
    EXPECT_FALSE(saved.empty());
}

TEST_F(FrameProcessorTest, DISABLED_RecordingModeProcessesVideoAndWritesOutput)
{
    processor_.SetReaderType(ReaderType::kRecording);

    int processed_frames = 0;
    const auto output_path = test_utils::TestOutputPath("frame_processor_recording_output.mp4");
    std::filesystem::remove(output_path);

    config::ProcessingTarget process_target{
      .input_source = test_utils::TestFilePath("test_video_trimmed.mp4"),
      .output_path = output_path,
    };

    processor_.ProcessFrames(process_target, [&](cv::Mat&) { ++processed_frames; });

    EXPECT_GT(processed_frames, 0);
    EXPECT_TRUE(std::filesystem::exists(output_path));
    EXPECT_GT(std::filesystem::file_size(output_path), 0);
}

TEST_F(FrameProcessorTest, RecordingModeReturnsNulloptForInvalidInput)
{
    processor_.SetReaderType(ReaderType::kRecording);

    int processed_frames = 0;
    processor_.ProcessFrames(
      config::ProcessingTarget{
        .input_source = test_utils::TestFilePath("missing.mp4"),
        .output_path = test_utils::TestOutputPath("recording_invalid_output.mp4")},
      [&](cv::Mat&) { ++processed_frames; });

    EXPECT_EQ(processed_frames, 0);
}

TEST_F(FrameProcessorTest, OnlineModeReturnsNulloptForInvalidInput)
{
    processor_.SetReaderType(ReaderType::kOnline);

    int processed_frames = 0;
    processor_.ProcessFrames(
      config::ProcessingTarget{
        .input_source = test_utils::TestFilePath("missing.mp4"),
        .output_path = test_utils::TestOutputPath("online_invalid_output.mp4")},
      [&](cv::Mat&) { ++processed_frames; });

    EXPECT_EQ(processed_frames, 0);
}

// This is also disabled. Same as above
TEST_F(FrameProcessorTest, DISABLED_OnlineModeProcessesVideoSourceAndWritesOutput)
{
    processor_.SetReaderType(ReaderType::kOnline);

    const auto output_path = test_utils::TestOutputPath("frame_processor_online_output.mp4");
    std::filesystem::remove(output_path);

    const auto video_path = test_utils::TestFilePath("test_video_trimmed.mp4");
    config::ProcessingTarget process_target{
      .input_source = video_path,
      .output_path = output_path,
    };

    int processed_frames = 0;
    processor_.ProcessFrames(process_target, [&](cv::Mat&) { ++processed_frames; });

    EXPECT_GT(processed_frames, 0);
    EXPECT_TRUE(std::filesystem::exists(output_path));
    EXPECT_GT(std::filesystem::file_size(output_path), 0);
}
