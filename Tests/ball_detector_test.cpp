#include <gtest/gtest.h>

#include <cmath>
#include <cstdlib>
#include <filesystem>
#include <opencv2/opencv.hpp>
#include <string>

#include "detector.hpp"
#include "detector_types.hpp"

namespace
{
const std::string kTestOutputDirectory{"Tests/test_outputs"};
constexpr double kVideoTestDurationSeconds{10.0};

bool IsVerboseTestRun()
{
    const char* const verbose_flag{std::getenv("FOOSBALL_TEST_VERBOSE")};
    return verbose_flag != nullptr && verbose_flag[0] != '\0' && verbose_flag[0] != '0';
}

void EnsureTestOutputDirectoryExists()
{
    ASSERT_TRUE(std::filesystem::create_directories(kTestOutputDirectory) ||
                std::filesystem::exists(kTestOutputDirectory));
}
}  // namespace

class BallDetectorTest : public ::testing::Test
{
  protected:
    cv::Mat frame;

    void SetUp() override
    {
        frame = cv::imread(detector_types::kInputImagePath, cv::IMREAD_COLOR);
        EnsureTestOutputDirectoryExists();
    }
};

TEST_F(BallDetectorTest, ImageLoads)
{
    ASSERT_FALSE(frame.empty()) << "Failed to load input image: "
                                << detector_types::kInputImagePath;
}

TEST_F(BallDetectorTest, BallDetectionRuns)
{
    ASSERT_FALSE(frame.empty());

    detect_ball(frame);

    EXPECT_FALSE(frame.empty());
    bool success{cv::imwrite(detector_types::kOutputImagePath, frame)};
    EXPECT_TRUE(success) << "Failed to write output image: " << detector_types::kOutputImagePath;
}

TEST_F(BallDetectorTest, VideoReadAndAnnotatedWrite)
{
    cv::VideoCapture input_video{detector_types::kInputVideoPath};
    ASSERT_TRUE(input_video.isOpened())
      << "Failed to open input video: " << detector_types::kInputVideoPath;

    const int frame_width{static_cast<int>(input_video.get(cv::CAP_PROP_FRAME_WIDTH))};
    const int frame_height{static_cast<int>(input_video.get(cv::CAP_PROP_FRAME_HEIGHT))};
    double input_fps{input_video.get(cv::CAP_PROP_FPS)};
    if (input_fps <= 0.0)
    {
        input_fps = 30.0;
    }
    const double output_fps{input_fps};
    const int max_frames{static_cast<int>(std::ceil(output_fps * kVideoTestDurationSeconds))};

    const cv::Size frame_size{frame_width, frame_height};
    const int fourcc{cv::VideoWriter::fourcc('a', 'v', 'c', '1')};
    cv::VideoWriter output_video{detector_types::kOutputVideoPath, fourcc, output_fps, frame_size};

    ASSERT_TRUE(output_video.isOpened()) << "Failed to open output video with avc1 codec at path: "
                                         << detector_types::kOutputVideoPath;
    const bool is_verbose{IsVerboseTestRun()};

    int processed_frames{0};
    cv::Mat video_frame;
    while (processed_frames < max_frames && input_video.read(video_frame))
    {
        if (is_verbose && (processed_frames % 10 == 0))
        {
            std::cout << "Processing frame " << processed_frames << std::endl;
        }

        cv::Mat annotated = detect_ball(video_frame);
        ASSERT_FALSE(annotated.empty());
        output_video.write(annotated);
        ++processed_frames;
    }

    EXPECT_GT(processed_frames, 0)
      << "Input video had no readable frames: " << detector_types::kInputVideoPath;
}
