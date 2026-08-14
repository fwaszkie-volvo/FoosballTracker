#ifndef FOOSBALL_TRACKER_APP_MODEL_INCLUDE_PRIVATE_RATINGS_RATINGS_SERVICE_HPP_
#define FOOSBALL_TRACKER_APP_MODEL_INCLUDE_PRIVATE_RATINGS_RATINGS_SERVICE_HPP_

#include <optional>
#include <vector>

#include "db_storage.hpp"
#include "model_types.hpp"
#include "player.hpp"
#include "ratings_types.hpp"

namespace ratings
{
class RatingsService
{
  public:
    void CreatePlayer(const model::Nickname& nickname);
    void RecordMatch(const MatchInput& match);
    std::optional<Player> GetPlayer(const model::Nickname& nickname) const;
    std::vector<MatchInput> GetMatchHistory() const;

  private:
    std::optional<model::PlayerMap> LoadMatchPlayers(const MatchInput& match) const;

    db::DbStorage storage_{};
};
}  // namespace ratings

#endif  // FOOSBALL_TRACKER_APP_MODEL_INCLUDE_PRIVATE_RATINGS_RATINGS_SERVICE_HPP_
