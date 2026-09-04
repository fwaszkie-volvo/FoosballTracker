#ifndef FOOSBALL_TRACKER_APP_MODEL_INCLUDE_PRIVATE_RATINGS_DB_DB_STORAGE_HPP_
#define FOOSBALL_TRACKER_APP_MODEL_INCLUDE_PRIVATE_RATINGS_DB_DB_STORAGE_HPP_

#include <leveldb/db.h>

#include <cstdint>
#include <memory>
#include <optional>
#include <vector>

#include "model_types.hpp"
#include "player.hpp"
#include "ratings_types.hpp"

namespace db
{
class DbStorage
{
  public:
    DbStorage();
    ~DbStorage() = default;

    DbStorage(const DbStorage&)            = delete;
    DbStorage& operator=(const DbStorage&) = delete;

    void CreatePlayer(const Nickname& nickname);
    bool TryUpdatePlayerElo(const Nickname& nickname, const int elo);
    std::optional<Player> GetPlayer(const Nickname& nickname) const;
    bool InsertMatch(const ratings::MatchInput& match);
    bool UpdateElos(const model::PlayerEloMap& new_elos);
    std::vector<ratings::MatchInput> GetMatchHistory() const;

  private:
    std::uint64_t GetNextMatchId() const;

    std::unique_ptr<leveldb::DB> db_;
};
}  // namespace db

#endif  // FOOSBALL_TRACKER_APP_MODEL_INCLUDE_PRIVATE_RATINGS_DB_DB_STORAGE_HPP_
