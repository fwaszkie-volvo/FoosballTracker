#include "player_ratings.hpp"

#include <array>
#include <optional>

namespace ratings
{
std::optional<Player> PlayerRatingsService::GetPlayer(const Nickname& nickname) const
{
    const auto it = players_.find(nickname);
    return (it == players_.end()) ? std::nullopt : std::optional<Player>{it->second};
}

void PlayerRatingsService::RecordMatch(const MatchInput& match)
{
    for (const auto& team : match.nicknames)
    {
        for (const auto& nickname : team)
        {
            GetOrCreatePlayer(nickname);
        }
    }

    const auto team_elos = ComputeTeamElos(match);
    ApplyMatchDeltas(match, team_elos);

    match_history_.push_back(match);
}

const Player& PlayerRatingsService::GetOrCreatePlayer(const Nickname& nickname)
{
    const auto [it, inserted] = players_.try_emplace(nickname, nickname, config_.initial_rating);
    std::ignore = inserted;
    return it->second;
}

TeamElos PlayerRatingsService::ComputeTeamElos(const MatchInput& match) const
{
    TeamElos team_elos{};
    for (std::size_t team_index{0U}; team_index < kTeamsNumber; ++team_index)
    {
        team_elos.at(team_index) = TeamAverageElo(match.nicknames[team_index]);
    }

    return team_elos;
}

double PlayerRatingsService::TeamAverageElo(const TeamNicknames& team) const
{
    double total_elo{0.0};
    for (const auto& nickname : team)
    {
        total_elo += static_cast<double>(players_.at(nickname).GetElo());
    }
    return total_elo / static_cast<double>(team.size());
}

void PlayerRatingsService::ApplyMatchDeltas(const MatchInput& match, const TeamElos& team_elos)
{
    for (std::size_t team_index{0U}; team_index < kTeamsNumber; ++team_index)
    {
        const int delta = team_delta_calculator_.ComputeTeamDelta(team_index, match, team_elos);
        for (const auto& nickname : match.nicknames.at(team_index))
        {
            auto& player = players_.at(nickname);
            player.SetElo(player.GetElo() + delta);
        }
    }
}
}  // namespace ratings
