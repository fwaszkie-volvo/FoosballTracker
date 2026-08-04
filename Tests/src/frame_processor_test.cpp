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
    processor_.ProcessFrames(test_utils::TestFilePath("ball_unobscured.jpg"),
                             [&](cv::Mat&) { ++processed_frames; });

    EXPECT_EQ(processed_frames, 0);
}

TEST_F(FrameProcessorTest, PhotoModeProcessesSigleFrame)
{
    processor_.SetReaderType(ReaderType::kPhoto);

    int processed_frames = 0;

    processor_.ProcessFrames(
      test_utils::TestFilePath("ball_unobscured.jpg"),
      [&](cv::Mat& frame)
      {
          ++processed_frames;
          cv::circle(
            frame, cv::Point(frame.cols / 2, frame.rows / 2), 10, cv::Scalar(0, 255, 0), 2);
      });

    EXPECT_EQ(processed_frames, 1);
}

TEST_F(FrameProcessorTest, RecordingModeProcessesVideo)
{
    processor_.SetReaderType(ReaderType::kRecording);

    int processed_frames = 0;

    processor_.ProcessFrames(test_utils::TestFilePath("test_video_trimmed.mp4"),
                             [&](cv::Mat&) { ++processed_frames; });

    EXPECT_GT(processed_frames, 0);
}

TEST_F(FrameProcessorTest, RecordingModeReturnsNulloptForInvalidInput)
{
    processor_.SetReaderType(ReaderType::kRecording);

    int processed_frames = 0;
    processor_.ProcessFrames(test_utils::TestFilePath("missing.mp4"),
                             [&](cv::Mat&) { ++processed_frames; });

    EXPECT_EQ(processed_frames, 0);
}

TEST_F(FrameProcessorTest, OnlineModeReturnsNulloptForInvalidInput)
{
    processor_.SetReaderType(ReaderType::kOnline);

    int processed_frames = 0;
    processor_.ProcessFrames(test_utils::TestFilePath("missing.mp4"),
                             [&](cv::Mat&) { ++processed_frames; });

    EXPECT_EQ(processed_frames, 0);
}

TEST_F(FrameProcessorTest, OnlineModeProcessesVideoSource)
{
    processor_.SetReaderType(ReaderType::kOnline);

    int processed_frames = 0;
    processor_.ProcessFrames(test_utils::TestFilePath("test_video_trimmed.mp4"),
                             [&](cv::Mat&) { ++processed_frames; });

    EXPECT_GT(processed_frames, 0);
}
