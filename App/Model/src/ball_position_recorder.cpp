#include "ball_position_recorder.hpp"

#include <cmath>
#include <cstddef>

#include "detector_types.hpp"

BallPositionRecorder::BallPositionRecorder()
{
    static constexpr double kAverageMatchDurationSeconds{1500.0};

    const std::size_t reserved_samples{static_cast<std::size_t>(
      std::ceil(kAverageMatchDurationSeconds * detector_types::kDefaultProcessingFps))};

    positions_.reserve(reserved_samples);
}

void BallPositionRecorder::RecordSample(const float ball_y_position)
{
    positions_.push_back(ball_y_position);
}
