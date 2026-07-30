#ifndef FOOSBALL_TRACKER_APP_MODEL_INCLUDE_PRIVATE_RATINGS_MATCH_STORAGE_HPP_
#define FOOSBALL_TRACKER_APP_MODEL_INCLUDE_PRIVATE_RATINGS_MATCH_STORAGE_HPP_

#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

#include "model_types.hpp"
#include "player.hpp"
#include "ratings_types.hpp"

namespace ratings
{
class MatchStorage
{
  public:
    bool CreatePlayer(const model::Nickname& nickname);
    bool RecordMatch(const MatchInput& match);

    std::optional<Player> GetPlayer(const model::Nickname& nickname) const;

    const std::vector<MatchInput>& GetMatchHistory() const { return match_history_; }

  private:
    bool HasAllPlayersRegistered(const MatchInput& match) const;
    void ApplyMatchDeltas(const MatchInput& match);
    MatchInput BuildMatchWithCurrentRatings(const MatchInput& match) const;

    std::unordered_map<model::Nickname, Player> players_{};
    std::vector<MatchInput> match_history_{};
};
}  // namespace ratings

#endif  // FOOSBALL_TRACKER_APP_MODEL_INCLUDE_PRIVATE_RATINGS_MATCH_STORAGE_HPP_
