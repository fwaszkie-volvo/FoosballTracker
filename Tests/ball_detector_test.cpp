#include "ball_detector.hpp"

#include <gtest/gtest.h>

#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <filesystem>
#include <iomanip>
#include <opencv2/opencv.hpp>
#include <sstream>
#include <string>

#include "detector_types.hpp"

namespace
{
constexpr double kVideoTestDurationSeconds{75.0};
constexpr double kVelocityArrowScale{8.0};
const cv::Scalar kVelocityArrowColor{0, 0, 255};
constexpr int kVelocityArrowThickness{2};
constexpr double kVelocityArrowTipLength{0.25};
constexpr int kOverlayLeftMargin{12};
constexpr int kOverlayTopMargin{12};
constexpr int kOverlayLineSpacing{22};
constexpr int kOverlayLineCount{3};
constexpr int kOverlayBottomPaddingLines{1};
constexpr int kOverlayFontFace{cv::FONT_HERSHEY_SIMPLEX};
constexpr double kOverlayFontScale{0.55};
constexpr int kOverlayTextThickness{1};
const cv::Scalar kOverlayBackgroundColor{0, 0, 0};
const cv::Scalar kOverlayTextColor{255, 255, 255};

std::string FormatMeasurementValue(const double value)
{
    std::ostringstream stream;
    stream << std::fixed << std::setprecision(2) << value;
    return stream.str();
}

void DrawMeasurementOverlay(cv::Mat& frame, const BallDetector::BallMeasurement& measurement)
{
    if (measurement.found)
    {
        const cv::Point center{cvRound(measurement.position.x), cvRound(measurement.position.y)};
        const cv::Point velocity_end{center +
                                     cv::Point{cvRound(measurement.speed.x * kVelocityArrowScale),
                                               cvRound(measurement.speed.y * kVelocityArrowScale)}};
        cv::arrowedLine(frame,
                        center,
                        velocity_end,
                        kVelocityArrowColor,
                        kVelocityArrowThickness,
                        cv::LINE_AA,
                        0,
                        kVelocityArrowTipLength);
    }

    const std::string found_line{std::string{"Found: "} + (measurement.found ? "true" : "false")};
    const std::string position_line{"Position: (" + FormatMeasurementValue(measurement.position.x) +
                                    ", " + FormatMeasurementValue(measurement.position.y) + ")"};
    const std::string speed_line{"Speed: (" + FormatMeasurementValue(measurement.speed.x) + ", " +
                                 FormatMeasurementValue(measurement.speed.y) + ")"};

    const std::string lines[]{found_line, position_line, speed_line};

    int max_text_width{0};
    int baseline{0};
    for (const auto& line : lines)
    {
        const cv::Size text_size{cv::getTextSize(
          line, kOverlayFontFace, kOverlayFontScale, kOverlayTextThickness, &baseline)};
        max_text_width = std::max(max_text_width, text_size.width);
    }

    const int overlay_width{max_text_width + 2 * kOverlayLeftMargin};
    const int overlay_height{
      (kOverlayLineCount + kOverlayBottomPaddingLines) * kOverlayLineSpacing + kOverlayTopMargin};
    cv::rectangle(
      frame, cv::Rect(0, 0, overlay_width, overlay_height), kOverlayBackgroundColor, cv::FILLED);

    int y{kOverlayTopMargin};
    for (const auto& line : lines)
    {
        cv::putText(frame,
                    line,
                    cv::Point{kOverlayLeftMargin, y},
                    kOverlayFontFace,
                    kOverlayFontScale,
                    kOverlayTextColor,
                    kOverlayTextThickness,
                    cv::LINE_AA);
        y += kOverlayLineSpacing;
    }
}
bool IsVerboseTestRun()
{
    const char* const verbose_flag{std::getenv("FOOSBALL_TEST_VERBOSE")};
    return verbose_flag != nullptr && verbose_flag[0] != '\0' && verbose_flag[0] != '0';
}

void EnsureTestOutputDirectoryExists()
{
    ASSERT_TRUE(std::filesystem::create_directories(detector_types::kTestOutputsDir) ||
                std::filesystem::exists(detector_types::kTestOutputsDir));
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

TEST_F(BallDetectorTest, BallMeasurementStoresPositionSpeedAndSize)
{
    ASSERT_FALSE(frame.empty());

    BallDetector detector;
    detector.Detect(frame);

    const auto first_measurement{detector.GetMeasurement()};
    EXPECT_TRUE(first_measurement.found);
    EXPECT_GE(first_measurement.position.x, 0.0);
    EXPECT_GE(first_measurement.position.y, 0.0);

    detector.Detect(frame);

    const auto second_measurement{detector.GetMeasurement()};
    EXPECT_TRUE(second_measurement.found);
    EXPECT_DOUBLE_EQ(second_measurement.position.x, first_measurement.position.x);
    EXPECT_DOUBLE_EQ(second_measurement.position.y, first_measurement.position.y);
    EXPECT_DOUBLE_EQ(second_measurement.speed.x, 0.0);
    EXPECT_DOUBLE_EQ(second_measurement.speed.y, 0.0);
}

TEST_F(BallDetectorTest, VideoReadAndMeasurementOverlayWrite)
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
    cv::VideoWriter output_video{
      detector_types::kBallMeasurementOverlayOutputPath, fourcc, output_fps, frame_size};

    ASSERT_TRUE(output_video.isOpened()) << "Failed to open output video with avc1 codec at path: "
                                         << detector_types::kBallMeasurementOverlayOutputPath;

    const bool is_verbose{IsVerboseTestRun()};

    BallDetector detector;
    cv::Mat video_frame;
    int processed_frames{0};
    int measured_frames{0};
    while (processed_frames < max_frames && input_video.read(video_frame))
    {
        if (is_verbose && (processed_frames % 10 == 0))
        {
            std::cout << "Processing frame " << processed_frames << std::endl;
        }

        detector.Detect(video_frame);
        detector.Draw(video_frame);
        const auto measurement{detector.GetMeasurement()};

        if (is_verbose)
        {
            DrawMeasurementOverlay(video_frame, measurement);
        }

        ASSERT_FALSE(video_frame.empty());
        output_video.write(video_frame);

        if (measurement.found)
        {
            ++measured_frames;
        }

        ++processed_frames;
    }

    EXPECT_GT(processed_frames, 0)
      << "Input video had no readable frames: " << detector_types::kInputVideoPath;
    EXPECT_GT(measured_frames, 0) << "No ball measurements were available for overlay output";

    EXPECT_TRUE(std::filesystem::exists(detector_types::kBallMeasurementOverlayOutputPath))
      << "Failed to create overlay output video: "
      << detector_types::kBallMeasurementOverlayOutputPath;
}
