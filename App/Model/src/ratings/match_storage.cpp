#include "match_storage.hpp"

#include <cstddef>
#include <utility>

#include "match_elo_calculator.hpp"

namespace ratings
{
std::optional<Player> MatchStorage::GetPlayer(const model::Nickname& nickname) const
{
    const auto it = players_.find(nickname);
    return (it == players_.end()) ? std::nullopt : std::optional<Player>{it->second};
}

bool MatchStorage::CreatePlayer(const model::Nickname& nickname)
{
    const auto [_, inserted] = players_.try_emplace(nickname, nickname, kDefaultElo);
    return inserted;
}

bool MatchStorage::RecordMatch(const MatchInput& match)
{
    if (!HasAllPlayersRegistered(match))
    {
        return false;
    }

    const MatchInput match_with_ratings = BuildMatchWithCurrentRatings(match);
    ApplyMatchDeltas(match_with_ratings);

    match_history_.push_back(match_with_ratings);
    return true;
}

bool MatchStorage::HasAllPlayersRegistered(const MatchInput& match) const
{
    for (const auto& team : match.teams_)
    {
        if (!players_.contains(team.players.first.GetNickname()) ||
            !players_.contains(team.players.second.GetNickname()))
        {
            return false;
        }
    }

    return true;
}

MatchInput MatchStorage::BuildMatchWithCurrentRatings(const MatchInput& match) const
{
    return MatchInput{
      .teams_ =
        {
          model::Team{.players = {players_.at(match.teams_.at(0).players.first.GetNickname()),
                                  players_.at(match.teams_.at(0).players.second.GetNickname())}},
          model::Team{.players = {players_.at(match.teams_.at(1).players.first.GetNickname()),
                                  players_.at(match.teams_.at(1).players.second.GetNickname())}},
        },
      .set_scores_ = match.set_scores_,
    };
}

void MatchStorage::ApplyMatchDeltas(const MatchInput& match)
{
    const auto deltas = calculator::ComputeMatchElos(match);

    for (std::size_t team_index{0U}; team_index < model::kTeamsNumber; ++team_index)
    {
        const int delta = deltas.at(team_index);

        auto& first_player = players_.at(match.teams_.at(team_index).players.first.GetNickname());
        first_player.SetElo(first_player.GetElo() + delta);

        auto& second_player = players_.at(match.teams_.at(team_index).players.second.GetNickname());
        second_player.SetElo(second_player.GetElo() + delta);
    }
}
}  // namespace ratings
