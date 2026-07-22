#include "frame_processor.hpp"

#include <gtest/gtest.h>

#include <filesystem>
#include <opencv2/opencv.hpp>
#include <string>

namespace
{
std::string TestFile(const std::string& file_name)
{
    return (std::filesystem::path(TEST_SOURCE_DIR) / "test_files" / file_name).string();
}

std::string TestOutput(const std::string& file_name)
{
    const auto output_dir = std::filesystem::path(TEST_SOURCE_DIR) / "test_outputs";
    std::filesystem::create_directories(output_dir);
    return (output_dir / file_name).string();
}
}  // namespace

TEST(FrameProcessorTest, UnspecifiedReaderTypeReturnsNullopt)
{
    FrameProcessor processor;

    int processed_frames = 0;
    const auto result = processor.ProcessFrames(TestFile("ball_unobscured.jpg"),
                                                TestOutput("unspecified_output.jpg"),
                                                [&](cv::Mat&) { ++processed_frames; });

    EXPECT_FALSE(result.has_value());
    EXPECT_EQ(processed_frames, 0);
}

TEST(FrameProcessorTest, PhotoModeProcessesSingleFrameAndWritesImage)
{
    FrameProcessor processor;
    processor.SetReaderType(ReaderType::kPhoto);

    int processed_frames = 0;
    const auto output_path = TestOutput("frame_processor_photo_output.jpg");
    std::filesystem::remove(output_path);

    const auto result = processor.ProcessFrames(
      TestFile("ball_unobscured.jpg"),
      output_path,
      [&](cv::Mat& frame)
      {
          ++processed_frames;
          cv::circle(
            frame, cv::Point(frame.cols / 2, frame.rows / 2), 10, cv::Scalar(0, 255, 0), 2);
      });

    ASSERT_TRUE(result.has_value());
    EXPECT_FALSE(result->empty());
    EXPECT_EQ(processed_frames, 1);
    EXPECT_TRUE(std::filesystem::exists(output_path));

    const auto saved = cv::imread(output_path);
    EXPECT_FALSE(saved.empty());
}

TEST(FrameProcessorTest, RecordingModeProcessesVideoAndWritesOutput)
{
    FrameProcessor processor;
    processor.SetReaderType(ReaderType::kRecording);

    int processed_frames = 0;
    const auto output_path = TestOutput("frame_processor_recording_output.mp4");
    std::filesystem::remove(output_path);

    const auto result = processor.ProcessFrames(
      TestFile("test_video.mp4"), output_path, [&](cv::Mat&) { ++processed_frames; });

    ASSERT_TRUE(result.has_value());
    EXPECT_FALSE(result->empty());
    EXPECT_GT(processed_frames, 0);
    EXPECT_TRUE(std::filesystem::exists(output_path));
    EXPECT_GT(std::filesystem::file_size(output_path), 0);
}

TEST(FrameProcessorTest, RecordingModeReturnsNulloptForInvalidInput)
{
    FrameProcessor processor;
    processor.SetReaderType(ReaderType::kRecording);

    int processed_frames = 0;
    const auto result = processor.ProcessFrames(TestFile("missing.mp4"),
                                                TestOutput("recording_invalid_output.mp4"),
                                                [&](cv::Mat&) { ++processed_frames; });

    EXPECT_FALSE(result.has_value());
    EXPECT_EQ(processed_frames, 0);
}

TEST(FrameProcessorTest, OnlineModeReturnsNulloptForInvalidInput)
{
    FrameProcessor processor;
    processor.SetReaderType(ReaderType::kOnline);

    int processed_frames = 0;
    const auto result = processor.ProcessFrames("rtsp://127.0.0.1:1/nonexistent",
                                                TestOutput("online_invalid_output.mp4"),
                                                [&](cv::Mat&) { ++processed_frames; });

    EXPECT_FALSE(result.has_value());
    EXPECT_EQ(processed_frames, 0);
}

TEST(FrameProcessorTest, OnlineModeProcessesVideoSourceAndWritesOutput)
{
    FrameProcessor processor;
    processor.SetReaderType(ReaderType::kOnline);

    int processed_frames = 0;
    const auto source = TestFile("test_video.mp4");
    const auto output_path = TestOutput("frame_processor_online_output.mp4");
    std::filesystem::remove(output_path);

    const auto result =
      processor.ProcessFrames(source, output_path, [&](cv::Mat&) { ++processed_frames; });

    ASSERT_TRUE(result.has_value());
    EXPECT_FALSE(result->empty());
    EXPECT_GT(processed_frames, 0);
    EXPECT_TRUE(std::filesystem::exists(output_path));
    EXPECT_GT(std::filesystem::file_size(output_path), 0);
}
