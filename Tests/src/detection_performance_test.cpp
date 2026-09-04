#include <gtest/gtest.h>
#include <spdlog/spdlog.h>

#include <chrono>
#include <numeric>
#include <opencv2/opencv.hpp>
#include <string>
#include <vector>

#include "frame_processor.hpp"
#include "goal_detector.hpp"
#include "reader_factory.hpp"
#include "utils.hpp"

namespace
{
using Clock = std::chrono::steady_clock;

double ElapsedMilliseconds(const Clock::time_point& start, const Clock::time_point& end)
{
    return std::chrono::duration<double, std::milli>(end - start).count();
}
}  // namespace

TEST(DISABLED_DetectionPerformance, CachedGoalDetectionIsFasterThanFirstFrame)
{
    const std::string input_file{test_utils::TestFilePath("test_video.mp4")};

    GoalDetector goal_detector{};
    FrameProcessor processor{};
    processor.SetReaderType(ReaderType::kRecording);

    std::vector<double> frame_durations_ms;

    processor.ProcessFrames(input_file,
                            [&](cv::Mat& frame) -> void
                            {
                                const auto start{Clock::now()};
                                goal_detector.Detect(frame);
                                const auto end{Clock::now()};
                                frame_durations_ms.push_back(ElapsedMilliseconds(start, end));
                            });

    ASSERT_GE(frame_durations_ms.size(), 2U) << "Not enough frames processed to compare timings";

    const double first_frame_ms{frame_durations_ms.front()};
    const double subsequent_frames_total_ms{
      std::accumulate(frame_durations_ms.begin() + 1, frame_durations_ms.end(), 0.0)};
    const double average_subsequent_frame_ms{subsequent_frames_total_ms /
                                             static_cast<double>(frame_durations_ms.size() - 1)};

    spdlog::info("First frame: {:.3f} ms, average of following {} frames: {:.3f} ms",
                 first_frame_ms,
                 frame_durations_ms.size() - 1,
                 average_subsequent_frame_ms);

    EXPECT_LT(average_subsequent_frame_ms, first_frame_ms);
}
