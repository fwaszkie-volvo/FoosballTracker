#include "db_storage.hpp"

#include <leveldb/iterator.h>
#include <leveldb/options.h>
#include <leveldb/slice.h>
#include <leveldb/status.h>
#include <leveldb/write_batch.h>
#include <spdlog/spdlog.h>

#include <algorithm>
#include <charconv>
#include <cstdint>
#include <format>
#include <map>
#include <nlohmann/json.hpp>
#include <nlohmann/json_fwd.hpp>
#include <optional>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

#include "converters.hpp"
#include "model_types.hpp"
#include "player.hpp"
#include "ratings_types.hpp"

namespace
{
constexpr std::string_view kMatchKeyPrefix{"match:"};
constexpr std::string_view kMatchKeyUpperBound{"match;"};

std::string PlayerKey(const model::Nickname& nickname)
{
    return std::format("player:{}", nickname);
}

std::string MatchKey(const std::uint64_t id)
{
    return std::format("{}{:020}", kMatchKeyPrefix, id);
}
}  // namespace

namespace db
{
DbStorage::DbStorage()
{
    static constexpr std::string_view kPath{"foosball_tracker.db"};

    leveldb::Options options{};
    options.create_if_missing = true;

    leveldb::DB* db{nullptr};
    const leveldb::Status status{leveldb::DB::Open(options, std::string{kPath}, &db)};
    if (!status.ok())
    {
        spdlog::error("Failed to open database at {}: {}", kPath, status.ToString());
    }
    db_.reset(db);
}

void DbStorage::CreatePlayer(const model::Nickname& nickname)
{
    if (GetPlayer(nickname).has_value())
    {
        return;
    }

    db_->Put(leveldb::WriteOptions(), PlayerKey(nickname), std::format("{}", kDefaultElo));
}

bool DbStorage::TryUpdatePlayerElo(const model::Nickname& nickname, const int elo)
{
    if (!GetPlayer(nickname).has_value())
    {
        return false;
    }

    return db_->Put(leveldb::WriteOptions(), PlayerKey(nickname), std::format("{}", elo)).ok();
}

std::optional<Player> DbStorage::GetPlayer(const model::Nickname& nickname) const
{
    std::string value{};
    if (!db_->Get(leveldb::ReadOptions(), PlayerKey(nickname), &value).ok())
    {
        return std::nullopt;
    }

    int elo{};
    std::from_chars(value.data(), value.data() + value.size(), elo);
    return Player{nickname, elo};
}

std::uint64_t DbStorage::GetNextMatchId() const
{
    const leveldb::Slice prefix{kMatchKeyPrefix.data(), kMatchKeyPrefix.size()};
    const leveldb::Slice upper_bound{kMatchKeyUpperBound.data(), kMatchKeyUpperBound.size()};

    const std::unique_ptr<leveldb::Iterator> it{db_->NewIterator(leveldb::ReadOptions())};
    it->Seek(upper_bound);
    it->Prev();

    if (!it->Valid() || !it->key().starts_with(prefix))
    {
        static constexpr std::uint64_t kFirstMatchId{1};
        return kFirstMatchId;
    }

    const std::string last_key{it->key().ToString()};
    const std::string_view suffix{last_key.data() + kMatchKeyPrefix.size(),
                                  last_key.size() - kMatchKeyPrefix.size()};

    std::uint64_t last_id{};
    std::from_chars(suffix.data(), suffix.data() + suffix.size(), last_id);
    return last_id + 1;
}

bool DbStorage::InsertMatch(const ratings::MatchInput& match, const model::PlayerEloMap& new_elos)
{
    const bool all_players_exist{std::ranges::all_of(
      new_elos, [this](const auto& entry) { return GetPlayer(entry.first).has_value(); })};
    if (!all_players_exist)
    {
        return false;
    }

    leveldb::WriteBatch batch{};
    batch.Put(MatchKey(GetNextMatchId()), convert::MatchToJson(match).dump());
    for (const auto& [nickname, elo] : new_elos)
    {
        batch.Put(PlayerKey(nickname), std::format("{}", elo));
    }

    return db_->Write(leveldb::WriteOptions(), &batch).ok();
}

std::vector<ratings::MatchInput> DbStorage::GetMatchHistory() const
{
    std::vector<ratings::MatchInput> history{};

    const leveldb::Slice prefix{kMatchKeyPrefix.data(), kMatchKeyPrefix.size()};
    const std::unique_ptr<leveldb::Iterator> it{db_->NewIterator(leveldb::ReadOptions())};
    for (it->Seek(prefix); it->Valid() && it->key().starts_with(prefix); it->Next())
    {
        history.push_back(convert::MatchFromJson(nlohmann::json::parse(it->value().ToString())));
    }

    return history;
}
}  // namespace db
