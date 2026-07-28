#ifndef FOOSBALL_TRACKER_APP_MODEL_INCLUDE_PUBLIC_MODEL_MAIN_HPP_
#define FOOSBALL_TRACKER_APP_MODEL_INCLUDE_PUBLIC_MODEL_MAIN_HPP_

#include <opencv2/core/mat.hpp>
#include <optional>
#include <vector>

#include "generator_types.hpp"
#include "player.hpp"
#include "player_ratings.hpp"
#include "ratings_types.hpp"

class ModelMain
{
  public:
    std::optional<cv::Mat> Calculate();
    std::optional<generator::TeamDrawResult> GenerateTeams(const generator::Players& players) const;

    void RecordMatch(const ratings::MatchInput& match);
    std::optional<Player> GetPlayer(const ratings::Nickname& nickname) const;
    const std::vector<ratings::MatchInput>& GetMatchHistory() const;

  private:
    ratings::PlayerRatingsService ratings_service_{};
};

#endif  // FOOSBALL_TRACKER_APP_MODEL_INCLUDE_PUBLIC_MODEL_MAIN_HPP_
