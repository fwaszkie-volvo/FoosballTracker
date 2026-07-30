#ifndef FOOSBALL_TRACKER_APP_MODEL_INCLUDE_PUBLIC_MODEL_MAIN_HPP_
#define FOOSBALL_TRACKER_APP_MODEL_INCLUDE_PUBLIC_MODEL_MAIN_HPP_

#include <opencv2/core/mat.hpp>
#include <optional>
#include <utility>
#include <vector>

#include "generator_types.hpp"
#include "match_storage.hpp"
#include "model_types.hpp"
#include "player.hpp"
#include "ratings_types.hpp"

class ModelMain
{
  public:
    std::optional<cv::Mat> Calculate();
    std::optional<model::Teams> GenerateTeamsRandom(const generator::Players& players) const;
    std::optional<model::Teams> GenerateTeamsByElo(const generator::Players& players) const;

    bool CreatePlayer(const model::Nickname& nickname);
    bool RecordMatch(const ratings::MatchInput& match);
    std::optional<Player> GetPlayer(const model::Nickname& nickname) const;
    const std::vector<ratings::MatchInput>& GetMatchHistory() const;

  private:
    ratings::MatchStorage storage_{};
};

#endif  // FOOSBALL_TRACKER_APP_MODEL_INCLUDE_PUBLIC_MODEL_MAIN_HPP_
