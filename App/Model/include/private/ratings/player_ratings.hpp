#ifndef FOOSBALL_TRACKER_APP_MODEL_INCLUDE_PRIVATE_RATINGS_PLAYER_RATINGS_HPP_
#define FOOSBALL_TRACKER_APP_MODEL_INCLUDE_PRIVATE_RATINGS_PLAYER_RATINGS_HPP_

#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

#include "player.hpp"
#include "ratings_types.hpp"
#include "team_delta_calculator.hpp"

namespace ratings
{
class PlayerRatingsService
{
  public:
    void RecordMatch(const MatchInput& match);

    std::optional<Player> GetPlayer(const Nickname& nickname) const;

    const std::vector<MatchInput>& GetMatchHistory() const { return match_history_; };

  private:
    const Player& GetOrCreatePlayer(const Nickname& nickname);
    TeamElos ComputeTeamElos(const MatchInput& match) const;
    void ApplyMatchDeltas(const MatchInput& match, const TeamElos& team_elos);

    double TeamAverageElo(const TeamNicknames& team) const;

    EloConfig config_{};
    TeamDeltaCalculator team_delta_calculator_{config_};
    std::unordered_map<Nickname, Player> players_{};
    std::vector<MatchInput> match_history_{};
};
}  // namespace ratings

#endif  // FOOSBALL_TRACKER_APP_MODEL_INCLUDE_PRIVATE_RATINGS_PLAYER_RATINGS_HPP_
