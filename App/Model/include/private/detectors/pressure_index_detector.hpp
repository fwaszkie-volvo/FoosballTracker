#ifndef FOOSBALL_TRACKER_APP_MODEL_INCLUDE_PRIVATE_DETECTORS_PRESSURE_INDEX_DETECTOR_HPP_
#define FOOSBALL_TRACKER_APP_MODEL_INCLUDE_PRIVATE_DETECTORS_PRESSURE_INDEX_DETECTOR_HPP_

#include <vector>

class PressureIndexDetector
{
  public:
    PressureIndexDetector();

    void RecordSample(float red_distance_from_goal, float blue_distance_from_goal);

    const std::vector<float>& GetRedPressure() const { return possession_red_; }
    const std::vector<float>& GetBluePressure() const { return possession_blue_; }

  private:
    std::vector<float> possession_red_;
    std::vector<float> possession_blue_;
};

#endif  // FOOSBALL_TRACKER_APP_MODEL_INCLUDE_PRIVATE_DETECTORS_PRESSURE_INDEX_DETECTOR_HPP_
