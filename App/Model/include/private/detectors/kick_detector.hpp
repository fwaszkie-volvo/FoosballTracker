#ifndef FOOSBALL_TRACKER_APP_MODEL_INCLUDE_PRIVATE_DETECTORS_KICK_DETECTOR_HPP_
#define FOOSBALL_TRACKER_APP_MODEL_INCLUDE_PRIVATE_DETECTORS_KICK_DETECTOR_HPP_

#include <opencv2/core/types.hpp>

#include "ball_detector.hpp"
#include "detector.hpp"
#include "players_detector.hpp"

namespace kick_detector
{

enum class PlayersEnum
{
    red_team_offense,
    red_team_defense,
    blue_team_offense,
    blue_team_defense
};

struct Kick
{
    PlayersEnum player_;
    bool on_target_;
    bool detected_;
};

class KickDetector : public Detector
{
  public:
    void Detect(const cv::Mat& frame) override;
    void Draw(cv::Mat& frame) const override;

    const Kick& GetKick() const { return kick_; };

    bool detected_once_{};

  private:
    Kick kick_;
    player_detector::PlayersDetector players_detector_{};
    BallDetector ball_detector_{};
    cv::Rect player_to_draw_{};

    void DetectKick(const int64_t frame_width);
    bool BallPositionMovedBackwards(const std::deque<cv::Point2f>& position_history);
    bool DetectBallProximityToPlayer(const std::vector<cv::Rect>& player,
                                     const BallDetector::BallMeasurement& ball_measurement,
                                     const int64_t frame_width,
                                     PlayersEnum players_enum);
    bool DetectBallProximityToPlayers(const player_detector::Players& players,
                                      const BallDetector::BallMeasurement& ball_measurement,
                                      const int64_t frame_width);
};
}  // namespace kick_detector
#endif  // FOOSBALL_TRACKER_APP_MODEL_INCLUDE_PRIVATE_DETECTORS_KICK_DETECTOR_HPP_
