#include "converters.hpp"

#include <array>
#include <cstddef>
#include <nlohmann/detail/json_ref.hpp>
#include <nlohmann/json.hpp>
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

model::Teams TeamsFromNames(const model::TeamNicknames& names)
{
    return std::make_pair(
      model::Team{std::make_pair(Player{names.at(0).first}, Player{names.at(0).second})},
      model::Team{std::make_pair(Player{names.at(1).first}, Player{names.at(1).second})});
}

ratings::TeamSettings TeamSettingsFromNames(const model::TeamSettingsNicknames& data)
{
    return {TeamsFromNames(data.at(0)),
            TeamsFromNames(data.at(1)),
            TeamsFromNames(data.at(2)),
            TeamsFromNames(data.at(3))};
}

model::TeamNicknames NamesFromTeams(const model::Teams& teams)
{
    return {model::NicknamePair{teams.first.players.first.GetNickname(),
                                teams.first.players.second.GetNickname()},
            model::NicknamePair{teams.second.players.first.GetNickname(),
                                teams.second.players.second.GetNickname()}};
}

model::TeamSettingsNicknames NamesFromTeamSettings(const ratings::TeamSettings& settings)
{
    model::TeamSettingsNicknames result{};
    for (std::size_t i{0}; i < model::kSetsPerMatch; ++i)
    {
        result.at(i) = NamesFromTeams(settings.at(i));
    }
    return result;
}
}  // namespace

namespace convert
{
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

nlohmann::json MatchToJson(const ratings::MatchInput& match)
{
    return nlohmann::json{
      {"teams", NamesFromTeams(match.teams_)},
      {"set_scores", match.set_scores_},
      {"team_settings", NamesFromTeamSettings(match.team_settings_)},
    };
}

ratings::MatchInput MatchFromJson(const nlohmann::json& json)
{
    return ratings::MatchInput{
      .teams_      = TeamsFromNames(json.at("teams").get<model::TeamNicknames>()),
      .set_scores_ = json.at("set_scores").get<ratings::SetScores>(),
      .team_settings_ =
        TeamSettingsFromNames(json.at("team_settings").get<model::TeamSettingsNicknames>()),
    };
}
}  // namespace convert
