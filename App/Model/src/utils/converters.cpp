#include "converters.hpp"

#include <array>
#include <cstddef>
#include <string>
#include <utility>

#include "player.hpp"

namespace
{
model::Teams TeamsFromPlayers(const model::Teams& teams, const model::PlayerMap& players)
{
    return model::Teams{
      model::Team{.players = {players.at(teams.first.players.first.GetNickname()),
                              players.at(teams.first.players.second.GetNickname())}},
      model::Team{.players = {players.at(teams.second.players.first.GetNickname()),
                              players.at(teams.second.players.second.GetNickname())}},
    };
}
}  // namespace

namespace convert
{
model::Team TeamFromNames(const model::NicknamePair& names)
{
    return model::Team{std::make_pair(Player{names.first}, Player{names.second})};
}

model::Teams TeamsFromNames(const model::TeamSettingsNicknames& data, const int setting_index)
{
    return std::make_pair(TeamFromNames(data.at(setting_index).at(0)),
                          TeamFromNames(data.at(setting_index).at(1)));
}

ratings::TeamSettings TeamSettingsFromNames(const model::TeamSettingsNicknames& data)
{
    return {TeamsFromNames(data, 0),
            TeamsFromNames(data, 1),
            TeamsFromNames(data, 2),
            TeamsFromNames(data, 3)};
}

ratings::MatchInput MatchInputFromPlayers(const ratings::MatchInput& match,
                                          const model::PlayerMap& players)
{
    const auto select_set = [&](std::size_t i) -> const model::Teams&
    {
        const auto& s = match.team_settings_.at(i);
        return !s.first.players.first.GetNickname().empty() ? s : match.teams_;
    };

    return ratings::MatchInput{
      .teams_         = TeamsFromPlayers(match.teams_, players),
      .set_scores_    = match.set_scores_,
      .team_settings_ = {TeamsFromPlayers(select_set(0), players),
                         TeamsFromPlayers(select_set(1), players),
                         TeamsFromPlayers(select_set(2), players),
                         TeamsFromPlayers(select_set(3), players)},
    };
}
}  // namespace convert
