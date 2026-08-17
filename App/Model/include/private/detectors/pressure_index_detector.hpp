#ifndef FOOSBALL_TRACKER_APP_MODEL_INCLUDE_PRIVATE_DETECTORS_PRESSURE_INDEX_DETECTOR_HPP_
#define FOOSBALL_TRACKER_APP_MODEL_INCLUDE_PRIVATE_DETECTORS_PRESSURE_INDEX_DETECTOR_HPP_

#include <vector>

class PressureIndexDetector
{
  public:
    PressureIndexDetector();

    void RecordSample(float ball_y_position);

    const std::vector<float>& GetBallPossession() const { return ball_possession_; }

  private:
    std::vector<float> ball_possession_;
};

#endif  // FOOSBALL_TRACKER_APP_MODEL_INCLUDE_PRIVATE_DETECTORS_PRESSURE_INDEX_DETECTOR_HPP_
