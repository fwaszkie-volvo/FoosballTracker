#include "match_storage.hpp"

#include <utility>

#include "match_elo_calculator.hpp"

namespace ratings
{
std::optional<Player> MatchStorage::GetPlayer(const model::Nickname& nickname) const
{
    if (!players_.contains(nickname))
    {
        return std::nullopt;
    }
    return players_.at(nickname);
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
    const auto has_team_players_registered = [this](const model::Team& team)
    {
        return players_.contains(team.players.first.GetNickname()) &&
               players_.contains(team.players.second.GetNickname());
    };

    return has_team_players_registered(match.teams_.first) &&
           has_team_players_registered(match.teams_.second);
}

MatchInput MatchStorage::BuildMatchWithCurrentRatings(const MatchInput& match) const
{
    static constexpr std::size_t kFirstSetIndex{0U};
    static constexpr std::size_t kSecondSetIndex{1U};
    static constexpr std::size_t kThirdSetIndex{2U};
    static constexpr std::size_t kFourthSetIndex{3U};

    const model::Teams& first_set  = SelectSetTeams(match, kFirstSetIndex);
    const model::Teams& second_set = SelectSetTeams(match, kSecondSetIndex);
    const model::Teams& third_set  = SelectSetTeams(match, kThirdSetIndex);
    const model::Teams& fourth_set = SelectSetTeams(match, kFourthSetIndex);

    const ratings::TeamSettings team_settings_with_ratings{
      BuildTeamsWithCurrentRatings(first_set),
      BuildTeamsWithCurrentRatings(second_set),
      BuildTeamsWithCurrentRatings(third_set),
      BuildTeamsWithCurrentRatings(fourth_set),
    };

    return MatchInput{
      .teams_         = BuildTeamsWithCurrentRatings(match.teams_),
      .set_scores_    = match.set_scores_,
      .team_settings_ = team_settings_with_ratings,
    };
}

const model::Teams& MatchStorage::SelectSetTeams(const MatchInput& match,
                                                 const std::size_t set_index)
{
    if (IsTeamSettingDefined(match.team_settings_.at(set_index)))
    {
        return match.team_settings_.at(set_index);
    }

    return match.teams_;
}

bool MatchStorage::IsTeamSettingDefined(const model::Teams& teams)
{
    return !teams.first.players.first.GetNickname().empty() &&
           !teams.first.players.second.GetNickname().empty() &&
           !teams.second.players.first.GetNickname().empty() &&
           !teams.second.players.second.GetNickname().empty();
}

model::Teams MatchStorage::BuildTeamsWithCurrentRatings(const model::Teams& teams) const
{
    return model::Teams{
      model::Team{.players = {players_.at(teams.first.players.first.GetNickname()),
                              players_.at(teams.first.players.second.GetNickname())}},
      model::Team{.players = {players_.at(teams.second.players.first.GetNickname()),
                              players_.at(teams.second.players.second.GetNickname())}},
    };
}

void MatchStorage::ApplyMatchDeltas(const MatchInput& match)
{
    const auto first_team_delta = calculator::ComputeFirstTeamEloDelta(match);

    const auto apply_team_delta = [this](const model::Team& team, const int delta)
    {
        auto& first_player = players_.at(team.players.first.GetNickname());
        first_player.SetElo(first_player.GetElo() + delta);

        auto& second_player = players_.at(team.players.second.GetNickname());
        second_player.SetElo(second_player.GetElo() + delta);
    };

    apply_team_delta(match.teams_.first, first_team_delta);
    apply_team_delta(match.teams_.second, -first_team_delta);
}
}  // namespace ratings
