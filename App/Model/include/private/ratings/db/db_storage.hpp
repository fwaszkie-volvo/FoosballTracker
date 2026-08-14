#ifndef FOOSBALL_TRACKER_APP_MODEL_INCLUDE_PRIVATE_RATINGS_DB_DB_STORAGE_HPP_
#define FOOSBALL_TRACKER_APP_MODEL_INCLUDE_PRIVATE_RATINGS_DB_DB_STORAGE_HPP_

#include <cstdint>
#include <optional>
#include <vector>

#include "model_types.hpp"
#include "player.hpp"
#include "ratings_types.hpp"
#include "sql_connection.hpp"

namespace db
{
class DbStorage
{
  public:
    DbStorage();
    ~DbStorage() = default;

    DbStorage(const DbStorage&)            = delete;
    DbStorage& operator=(const DbStorage&) = delete;

    void CreatePlayer(const model::Nickname& nickname);
    bool TryUpdatePlayerElo(const model::Nickname& nickname, const int elo);
    std::optional<Player> GetPlayer(const model::Nickname& nickname) const;
    bool InsertMatch(const ratings::MatchInput& match, const model::PlayerEloMap& new_elos);
    std::vector<ratings::MatchInput> GetMatchHistory() const;

  private:
    bool TryInsertMatchTeams(const std::int64_t match_id, const model::Teams& teams) const;
    bool TryInsertSetScores(const std::int64_t match_id, const ratings::SetScores& scores) const;
    bool TryInsertTeamSettings(const std::int64_t match_id,
                               const ratings::TeamSettings& settings) const;

    bool RollbackOnFailure(const bool operation_succeeded);

    std::optional<model::Teams> FetchMatchTeams(const std::int64_t match_id) const;
    ratings::TeamSettings FetchTeamSettings(const std::int64_t match_id) const;
    ratings::SetScores FetchSetScores(const std::int64_t match_id) const;

    SqlConnection connection_;
};
}  // namespace db

#endif  // FOOSBALL_TRACKER_APP_MODEL_INCLUDE_PRIVATE_RATINGS_DB_DB_STORAGE_HPP_
