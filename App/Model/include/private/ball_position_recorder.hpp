#ifndef FOOSBALL_TRACKER_APP_MODEL_INCLUDE_PRIVATE_BALL_POSITION_RECORDER_HPP_
#define FOOSBALL_TRACKER_APP_MODEL_INCLUDE_PRIVATE_BALL_POSITION_RECORDER_HPP_

#include <vector>

class BallPositionRecorder
{
  public:
    BallPositionRecorder();

    void RecordSample(float ball_y_position);

    const std::vector<float>& GetPositions() const { return positions_; }

  private:
    std::vector<float> positions_;
};

#endif  // FOOSBALL_TRACKER_APP_MODEL_INCLUDE_PRIVATE_BALL_POSITION_RECORDER_HPP_
