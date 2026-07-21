#include <gtest/gtest.h>

#include <cmath>
#include <cstdlib>
#include <filesystem>
#include <opencv2/opencv.hpp>

#include "detector.hpp"
#include "detector_config.hpp"

namespace
{
bool IsVerboseTestRun()
{
    const char* const verbose_flag = std::getenv("FOOSBALL_TEST_VERBOSE");
    return verbose_flag != nullptr && verbose_flag[0] != '\0' && verbose_flag[0] != '0';
}
}  // namespace

class BallDetectorTest : public ::testing::Test
{
  protected:
    cv::Mat frame;

    void SetUp() override
    {
        frame = cv::imread(detector_config::kInputImagePath, cv::IMREAD_COLOR);
    }
};

TEST_F(BallDetectorTest, image_loads)
{
    ASSERT_FALSE(frame.empty()) << "Failed to load input image: "
                                << detector_config::kInputImagePath;
}

TEST_F(BallDetectorTest, ball_detection_runs)
{
    ASSERT_FALSE(frame.empty());
    ASSERT_TRUE(std::filesystem::create_directories("Tests/test_outputs") ||
                std::filesystem::exists("Tests/test_outputs"));

    detect_ball(frame);

    EXPECT_FALSE(frame.empty());
    bool success = cv::imwrite(detector_config::kOutputImagePath, frame);
    EXPECT_TRUE(success) << "Failed to write output image: " << detector_config::kOutputImagePath;
}

TEST_F(BallDetectorTest, video_read_and_annotated_write)
{
    cv::VideoCapture input_video(detector_config::kInputVideoPath);
    ASSERT_TRUE(input_video.isOpened())
      << "Failed to open input video: " << detector_config::kInputVideoPath;

    const int frame_width = static_cast<int>(input_video.get(cv::CAP_PROP_FRAME_WIDTH));
    const int frame_height = static_cast<int>(input_video.get(cv::CAP_PROP_FRAME_HEIGHT));
    double input_fps = input_video.get(cv::CAP_PROP_FPS);
    if (input_fps <= 0.0)
    {
        input_fps = 30.0;
    }
    const double output_fps = input_fps;

    const cv::Size frame_size(frame_width, frame_height);
    const int fourcc = cv::VideoWriter::fourcc('a', 'v', 'c', '1');
    ASSERT_TRUE(std::filesystem::create_directories("Tests/test_outputs") ||
                std::filesystem::exists("Tests/test_outputs"));
    cv::VideoWriter output_video(detector_config::kOutputVideoPath, fourcc, output_fps, frame_size);

    ASSERT_TRUE(output_video.isOpened()) << "Failed to open output video with avc1 codec at path: "
                                         << detector_config::kOutputVideoPath;
    const bool is_verbose = IsVerboseTestRun();

    int processed_frames = 0;
    cv::Mat video_frame;
    while (input_video.read(video_frame))
    {
        if (is_verbose && (processed_frames + 1) % 10 == 0)
        {
            std::cout << "Processing frame " << (processed_frames + 1) << std::endl;
        }

        cv::Mat annotated = detect_ball(video_frame);
        ASSERT_FALSE(annotated.empty());
        output_video.write(annotated);
        ++processed_frames;
    }

    EXPECT_GT(processed_frames, 0)
      << "Input video had no readable frames: " << detector_config::kInputVideoPath;
}
