#include "pressure_index_detector.hpp"

#include <cmath>
#include <cstddef>

#include "detector_types.hpp"

PressureIndexDetector::PressureIndexDetector()
{
    static constexpr double kAverageMatchDurationSeconds{1500.0};

    const std::size_t reserved_samples{static_cast<std::size_t>(
      std::ceil(kAverageMatchDurationSeconds * detector_types::kDefaultProcessingFps))};

    ball_possession_.reserve(reserved_samples);
}

void PressureIndexDetector::RecordSample(const float ball_y_position)
{
    ball_possession_.push_back(ball_y_position);
}
