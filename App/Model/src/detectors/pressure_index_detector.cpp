#include "pressure_index_detector.hpp"

#include <cmath>
#include <cstddef>

#include "detector_types.hpp"

PressureIndexDetector::PressureIndexDetector()
{
    static constexpr double kAverageMatchDurationSeconds{1500.0};

    const std::size_t reserved_samples{static_cast<std::size_t>(
      std::ceil(kAverageMatchDurationSeconds * detector_types::kDefaultProcessingFps))};

    possession_red_.reserve(reserved_samples);
    possession_blue_.reserve(reserved_samples);
}

void PressureIndexDetector::RecordSample(const float red_distance_from_goal,
                                         const float blue_distance_from_goal)
{
    possession_red_.push_back(red_distance_from_goal);
    possession_blue_.push_back(blue_distance_from_goal);
}
