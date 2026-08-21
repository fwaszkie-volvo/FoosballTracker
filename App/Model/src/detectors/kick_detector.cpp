#include "kick_detector.hpp"

namespace kick_detector
{

bool KickDetector::DetectBallProximityToPlayers(
  const player_detector::Players& players,
  const BallDetector::BallMeasurement& ball_measurement,
  const int64_t frame_width)
{
    return DetectBallProximityToPlayer(players.red_team_.offense_,
                                       ball_measurement,
                                       frame_width,
                                       PlayersEnum::red_team_offense) ||
           DetectBallProximityToPlayer(players.red_team_.defense_,
                                       ball_measurement,
                                       frame_width,
                                       PlayersEnum::red_team_defense) ||
           DetectBallProximityToPlayer(players.blue_team_.offense_,
                                       ball_measurement,
                                       frame_width,
                                       PlayersEnum::blue_team_offense) ||
           DetectBallProximityToPlayer(players.blue_team_.defense_,
                                       ball_measurement,
                                       frame_width,
                                       PlayersEnum::blue_team_defense);
}

bool KickDetector::DetectBallProximityToPlayer(
  const std::vector<cv::Rect>& player,
  const BallDetector::BallMeasurement& ball_measurement,
  const int64_t frame_width,
  PlayersEnum players_enum)
{
    bool touch_detected{false};
    int64_t distance_squared{};

    for (auto player_piece : player)
    {
        cv::Point2d player_position{player_piece.x + player_piece.width / 2,
                                    player_piece.y + player_piece.height / 2};
        cv::Point2d ball_position{ball_measurement.position};

        constexpr int64_t width_to_ball_near_player_size_ratio =
          16;  // 580 / 38 - got this very crudely from paint
        const int64_t distance_threshold = frame_width / width_to_ball_near_player_size_ratio;
        distance_squared =
          (player_position.x - ball_position.x) * (player_position.x - ball_position.x) +
          (player_position.y - ball_position.y) * (player_position.y - ball_position.y);

        if (distance_squared < distance_threshold)
        {
            touch_detected  = true;
            kick_.player_   = players_enum;
            player_to_draw_ = player_piece;
            detected_once_  = true;
        }
    }

    return touch_detected;
}

bool KickDetector::BallPositionMovedBackwards(const std::deque<cv::Point2f>& position_history)
{
    bool ball_velocity_switch_detected{false};
    cv::Point2f last_position        = position_history[position_history.size() - 1];
    cv::Point2f second_last_position = position_history[position_history.size() - 2];
    cv::Point2f third_last_position  = position_history[position_history.size() - 3];

    // [----|-o--------] third_last_position
    // [----|o---------] second_last_position
    // [----|-o--------] last_position
    bool ball_switched_from_left_to_right{second_last_position.x <= last_position.x &&
                                          second_last_position.x <= third_last_position.x};
    // [-----o-|-------] third_last_position
    // [------o|------] second_last_position
    // [-----o-|------] last_position
    bool ball_switched_from_right_to_left{second_last_position.x >= last_position.x &&
                                          second_last_position.x >= third_last_position.x};

    if (ball_switched_from_left_to_right || ball_switched_from_right_to_left)
    {
        ball_velocity_switch_detected = true;
    }

    return ball_velocity_switch_detected;
}

void KickDetector::DetectKick(const int64_t frame_width)
{
    bool kick_detected{false};
    const player_detector::Players players               = players_detector_.GetPlayers();
    const BallDetector::BallMeasurement ball_measurement = ball_detector_.GetMeasurement();
    const std::deque<cv::Point2f> position_history       = ball_detector_.GetPositionHistory();

    kick_.detected_ = DetectBallProximityToPlayers(players, ball_measurement, frame_width) &&
                      BallPositionMovedBackwards(position_history);

    // return kick_detected;
}

void KickDetector::Detect(const cv::Mat& frame)
{
    players_detector_.Detect(frame);
    ball_detector_.Detect(frame);
    DetectKick(frame.size().width);
}

void KickDetector::Draw(cv::Mat& frame) const
{
    cv::rectangle(
      frame, player_to_draw_, detector_types::kPlayersDrawRedColor, detector_types::kDrawThickness);
}
}  // namespace kick_detector