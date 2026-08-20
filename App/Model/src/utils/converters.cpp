#include "converters.hpp"

#include <cstddef>
#include <nlohmann/detail/json_ref.hpp>
#include <nlohmann/json.hpp>

#include "player.hpp"

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

    const auto make_team = [&](const model::Team& team)
    {
        return model::Team{.players = {players.at(team.players.first.GetNickname()),
                                       players.at(team.players.second.GetNickname())}};
    };
    const auto make_teams = [&](const model::Teams& teams)
    { return model::Teams{make_team(teams.first), make_team(teams.second)}; };

    return ratings::MatchInput{.teams_         = make_teams(match.teams_),
                               .set_scores_    = match.set_scores_,
                               .team_settings_ = {make_teams(select_set(0)),
                                                  make_teams(select_set(1)),
                                                  make_teams(select_set(2)),
                                                  make_teams(select_set(3))}};
}

nlohmann::json MatchToJson(const ratings::MatchInput& match)
{
    const auto names_from_teams = [](const model::Teams& teams)
    {
        return model::TeamNicknames{
          {{teams.first.players.first.GetNickname(), teams.first.players.second.GetNickname()},
           {teams.second.players.first.GetNickname(), teams.second.players.second.GetNickname()}}};
    };

    nlohmann::json result{};
    result["teams"]         = names_from_teams(match.teams_);
    result["set_scores"]    = match.set_scores_;
    result["team_settings"] = nlohmann::json::array({names_from_teams(match.team_settings_.at(0)),
                                                     names_from_teams(match.team_settings_.at(1)),
                                                     names_from_teams(match.team_settings_.at(2)),
                                                     names_from_teams(match.team_settings_.at(3))});
    return result;
}

ratings::MatchInput MatchFromJson(const nlohmann::json& json)
{
    const auto teams{json.at("teams").get<model::TeamNicknames>()};
    const auto settings{json.at("team_settings").get<model::TeamSettingsNicknames>()};
    const auto make_teams = [](const model::TeamNicknames& names)
    {
        return model::Teams{
          model::Team{.players = {Player{names.at(0).first}, Player{names.at(0).second}}},
          model::Team{.players = {Player{names.at(1).first}, Player{names.at(1).second}}}};
    };

    return ratings::MatchInput{
      .teams_         = make_teams(teams),
      .set_scores_    = json.at("set_scores").get<ratings::SetScores>(),
      .team_settings_ = {make_teams(settings.at(0)),
                         make_teams(settings.at(1)),
                         make_teams(settings.at(2)),
                         make_teams(settings.at(3))},
    };
}
}  // namespace convert
