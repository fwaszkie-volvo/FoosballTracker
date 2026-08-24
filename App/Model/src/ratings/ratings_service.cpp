#include "ratings_service.hpp"

#include <initializer_list>
#include <string>
#include <utility>

#include "converters.hpp"
#include "match_elo_calculator.hpp"

namespace ratings
{
std::optional<model::PlayerMap> RatingsService::LoadMatchPlayers(const MatchInput& match) const
{
    model::PlayerMap players_by_nickname{};
    const auto& match_teams{match.teams_};

    for (const auto& nickname : {match_teams.first.players.first.GetNickname(),
                                 match_teams.first.players.second.GetNickname(),
                                 match_teams.second.players.first.GetNickname(),
                                 match_teams.second.players.second.GetNickname()})
    {
        const auto player{storage_.GetPlayer(nickname)};
        if (!player.has_value())
        {
            return std::nullopt;
        }

        players_by_nickname.emplace(nickname, *player);
    }

    return players_by_nickname;
}

void RatingsService::CreatePlayer(const model::Nickname& nickname)
{
    storage_.CreatePlayer(nickname);
}

void RatingsService::RecordMatch(const MatchInput& match)
{
    const auto players_by_nickname{LoadMatchPlayers(match)};
    if (!players_by_nickname.has_value())
    {
        return;
    }

    const auto rated_match{convert::MatchInputFromPlayers(match, *players_by_nickname)};
    const int first_team_elo_delta{calculator::ComputeFirstTeamEloDelta(rated_match)};
    const auto& first_team_players{rated_match.teams_.first.players};
    const auto& second_team_players{rated_match.teams_.second.players};
    const model::PlayerEloMap updated_elos{
      {first_team_players.first.GetNickname(),
       first_team_players.first.GetElo() + first_team_elo_delta},
      {first_team_players.second.GetNickname(),
       first_team_players.second.GetElo() + first_team_elo_delta},
      {second_team_players.first.GetNickname(),
       second_team_players.first.GetElo() - first_team_elo_delta},
      {second_team_players.second.GetNickname(),
       second_team_players.second.GetElo() - first_team_elo_delta},
    };

    storage_.InsertMatch(rated_match);
    storage_.UpdateElos(updated_elos);
}

std::optional<Player> RatingsService::GetPlayer(const model::Nickname& nickname) const
{
    return storage_.GetPlayer(nickname);
}

std::vector<MatchInput> RatingsService::GetMatchHistory() const
{
    return storage_.GetMatchHistory();
}
}  // namespace ratings
