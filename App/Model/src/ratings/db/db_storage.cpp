#include "db_storage.hpp"

#include <array>
#include <cstddef>
#include <cstdint>
#include <optional>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include "converters.hpp"
#include "db_schema.hpp"
#include "model_types.hpp"
#include "player.hpp"
#include "ratings_types.hpp"
#include "sql_connection.hpp"
#include "sql_statement.hpp"

namespace db
{
DbStorage::DbStorage() : connection_{"foosball_tracker.db"} { connection_.ExecuteSql(kSchema); }

void DbStorage::CreatePlayer(const model::Nickname& nickname)
{
    static constexpr int kNicknameIndex{1};
    static constexpr int kEloIndex{2};

    auto statement{
      connection_.CreateStatement("INSERT OR IGNORE INTO players (nickname, elo) VALUES (?, ?)")};

    if (!statement.TryBindText(kNicknameIndex, nickname) ||
        !statement.TryBindInt(kEloIndex, kDefaultElo))
    {
        return;
    }

    statement.TryExecute();
}

bool DbStorage::TryUpdatePlayerElo(const model::Nickname& nickname, const int elo)
{
    static constexpr int kEloIndex{1};
    static constexpr int kNicknameIndex{2};

    auto statement{connection_.CreateStatement("UPDATE players SET elo = ? WHERE nickname = ?")};

    if (!statement.TryBindInt(kEloIndex, elo) || !statement.TryBindText(kNicknameIndex, nickname) ||
        !statement.TryExecute())
    {
        return false;
    }

    return connection_.GetAffectedRows() > 0;
}

std::optional<Player> DbStorage::GetPlayer(const model::Nickname& nickname) const
{
    static constexpr int kNicknameIndex{1};
    static constexpr int kEloColumn{0};

    auto statement{connection_.CreateStatement("SELECT elo FROM players WHERE nickname = ?")};

    if (!statement.TryBindText(kNicknameIndex, nickname) || !statement.TryFetchRow())
    {
        return std::nullopt;
    }

    return Player{nickname, statement.GetColumnInt(kEloColumn)};
}

bool DbStorage::TryInsertMatchTeams(const std::int64_t match_id, const model::Teams& teams) const
{
    static constexpr int kMatchIdIndex{1};
    static constexpr int kTeamIndexIndex{2};
    static constexpr int kPlayer1Index{3};
    static constexpr int kPlayer2Index{4};

    const std::array<model::Team, model::kTeamsNumber> all_teams{teams.first, teams.second};
    for (int team_index{0}; team_index < model::kTeamsNumber; ++team_index)
    {
        const auto& team{all_teams[team_index]};
        auto statement{connection_.CreateStatement(
          "INSERT INTO match_teams (match_id, team_index, player1, player2) VALUES (?, ?, ?, ?)")};
        if (!statement.TryBindInt64(kMatchIdIndex, match_id) ||
            !statement.TryBindInt(kTeamIndexIndex, team_index) ||
            !statement.TryBindText(kPlayer1Index, team.players.first.GetNickname()) ||
            !statement.TryBindText(kPlayer2Index, team.players.second.GetNickname()) ||
            !statement.TryExecute())
        {
            return false;
        }
    }
    return true;
}

bool DbStorage::TryInsertSetScores(const std::int64_t match_id,
                                   const ratings::SetScores& scores) const
{
    static constexpr int kMatchIdIndex{1};
    static constexpr int kTeamIndexIndex{2};
    static constexpr int kSetIndexIndex{3};
    static constexpr int kGoalsIndex{4};

    for (std::size_t team_index{0}; team_index < model::kTeamsNumber; ++team_index)
    {
        for (std::size_t set_index{0}; set_index < model::kSetsPerMatch; ++set_index)
        {
            auto statement{connection_.CreateStatement(
              "INSERT INTO set_scores (match_id, team_index, set_index, goals) VALUES (?,?,?,?)")};
            if (!statement.TryBindInt64(kMatchIdIndex, match_id) ||
                !statement.TryBindInt(kTeamIndexIndex, static_cast<int>(team_index)) ||
                !statement.TryBindInt(kSetIndexIndex, static_cast<int>(set_index)) ||
                !statement.TryBindInt(kGoalsIndex, scores.at(team_index).at(set_index)) ||
                !statement.TryExecute())
            {
                return false;
            }
        }
    }
    return true;
}

bool DbStorage::TryInsertTeamSettings(const std::int64_t match_id,
                                      const ratings::TeamSettings& settings) const
{
    static constexpr int kMatchIdIndex{1};
    static constexpr int kSetIndexIndex{2};
    static constexpr int kTeamIndexIndex{3};
    static constexpr int kPlayer1Index{4};
    static constexpr int kPlayer2Index{5};

    for (std::size_t set_index{0}; set_index < model::kSetsPerMatch; ++set_index)
    {
        const std::array<model::Team, model::kTeamsNumber> teams{settings.at(set_index).first,
                                                                 settings.at(set_index).second};
        for (int team_index = 0; team_index < model::kTeamsNumber; ++team_index)
        {
            const auto& team{teams[team_index]};
            auto statement{connection_.CreateStatement(
              "INSERT INTO team_settings (match_id, setting_index, team_index, player1, player2) "
              "VALUES (?, ?, ?, ?, ?)")};
            if (!statement.TryBindInt64(kMatchIdIndex, match_id) ||
                !statement.TryBindInt(kSetIndexIndex, static_cast<int>(set_index)) ||
                !statement.TryBindInt(kTeamIndexIndex, team_index) ||
                !statement.TryBindText(kPlayer1Index, team.players.first.GetNickname()) ||
                !statement.TryBindText(kPlayer2Index, team.players.second.GetNickname()) ||
                !statement.TryExecute())
            {
                return false;
            }
        }
    }
    return true;
}

bool DbStorage::InsertMatch(const ratings::MatchInput& match, const model::PlayerEloMap& new_elos)
{
    if (!connection_.TryExecuteSql("BEGIN"))
    {
        return false;
    }

    auto header{connection_.CreateStatement("INSERT INTO matches DEFAULT VALUES")};
    if (!RollbackOnFailure(header.TryExecute()))
    {
        return false;
    }

    const std::int64_t match_id{connection_.LastInsertRowid()};

    if (!RollbackOnFailure(TryInsertMatchTeams(match_id, match.teams_) &&
                           TryInsertSetScores(match_id, match.set_scores_) &&
                           TryInsertTeamSettings(match_id, match.team_settings_)))
    {
        return false;
    }

    for (const auto& [nickname, elo] : new_elos)
    {
        if (!RollbackOnFailure(TryUpdatePlayerElo(nickname, elo)))
        {
            return false;
        }
    }

    return RollbackOnFailure(connection_.TryExecuteSql("COMMIT"));
}

bool DbStorage::RollbackOnFailure(const bool operation_succeeded)
{
    if (!operation_succeeded)
    {
        connection_.ExecuteSql("ROLLBACK");
    }
    return operation_succeeded;
}

std::optional<model::Teams> DbStorage::FetchMatchTeams(const std::int64_t match_id) const
{
    static constexpr int kMatchIdIndex{1};
    static constexpr int kPlayer1Column{0};
    static constexpr int kPlayer2Column{1};

    auto statement{
      connection_.CreateStatement("SELECT player1, player2 FROM match_teams "
                                  "WHERE match_id = ? ORDER BY team_index ASC")};
    if (!statement.TryBindInt64(kMatchIdIndex, match_id) || !statement.TryFetchRow())
    {
        return std::nullopt;
    }
    const model::NicknamePair first_team_names{statement.GetColumnString(kPlayer1Column),
                                               statement.GetColumnString(kPlayer2Column)};

    if (!statement.TryFetchRow())
    {
        return std::nullopt;
    }
    const model::NicknamePair second_team_names{statement.GetColumnString(kPlayer1Column),
                                                statement.GetColumnString(kPlayer2Column)};

    return std::make_pair(convert::TeamFromNames(first_team_names),
                          convert::TeamFromNames(second_team_names));
}

ratings::TeamSettings DbStorage::FetchTeamSettings(const std::int64_t match_id) const
{
    static constexpr int kMatchIdIndex{1};
    static constexpr int kSetIndexColumn{0};
    static constexpr int kTeamIndexColumn{1};
    static constexpr int kPlayer1Column{2};
    static constexpr int kPlayer2Column{3};

    model::TeamSettingsNicknames team_settings_player_names{};
    auto statement{connection_.CreateStatement(
      "SELECT setting_index, team_index, player1, player2 FROM team_settings "
      "WHERE match_id = ? ORDER BY setting_index ASC, team_index ASC")};
    if (statement.TryBindInt64(kMatchIdIndex, match_id))
    {
        while (statement.TryFetchRow())
        {
            const int set_index{statement.GetColumnInt(kSetIndexColumn)};
            const int team_index{statement.GetColumnInt(kTeamIndexColumn)};
            if (set_index >= 0 && set_index < static_cast<int>(model::kSetsPerMatch) &&
                team_index >= 0 && team_index < static_cast<int>(model::kTeamsNumber))
            {
                team_settings_player_names.at(set_index).at(team_index) = {
                  statement.GetColumnString(kPlayer1Column),
                  statement.GetColumnString(kPlayer2Column)};
            }
        }
    }

    return convert::TeamSettingsFromNames(team_settings_player_names);
}

ratings::SetScores DbStorage::FetchSetScores(const std::int64_t match_id) const
{
    static constexpr int kMatchIdIndex{1};
    static constexpr int kTeamIndexColumn{0};
    static constexpr int kSetIndexColumn{1};
    static constexpr int kGoalsColumn{2};

    ratings::SetScores set_scores{};
    auto statement{
      connection_.CreateStatement("SELECT team_index, set_index, goals FROM set_scores "
                                  "WHERE match_id = ? ORDER BY team_index ASC, set_index ASC")};
    if (statement.TryBindInt64(kMatchIdIndex, match_id))
    {
        while (statement.TryFetchRow())
        {
            set_scores.at(statement.GetColumnInt(kTeamIndexColumn))
              .at(statement.GetColumnInt(kSetIndexColumn)) =
              static_cast<std::uint8_t>(statement.GetColumnInt(kGoalsColumn));
        }
    }

    return set_scores;
}

std::vector<ratings::MatchInput> DbStorage::GetMatchHistory() const
{
    static constexpr int kMatchesIdColumn{0};

    std::vector<ratings::MatchInput> history{};
    auto matches_statement{connection_.CreateStatement("SELECT id FROM matches ORDER BY id ASC")};
    while (matches_statement.TryFetchRow())
    {
        const std::int64_t match_id{matches_statement.GetColumnInt64(kMatchesIdColumn)};

        const std::optional<model::Teams> match_teams{FetchMatchTeams(match_id)};
        if (!match_teams.has_value())
        {
            continue;
        }

        history.push_back(ratings::MatchInput{.teams_         = *match_teams,
                                              .set_scores_    = FetchSetScores(match_id),
                                              .team_settings_ = FetchTeamSettings(match_id)});
    }

    return history;
}
}  // namespace db
