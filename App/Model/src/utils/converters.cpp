#include "converters.hpp"

#include <array>
#include <cstdint>
#include <nlohmann/detail/json_ref.hpp>
#include <nlohmann/json.hpp>
#include <string>
#include <utility>

#include "model_types.hpp"
#include "player.hpp"
#include "ratings_types.hpp"

namespace convert
{
ratings::MatchInput MatchInputFromPlayers(const ratings::MatchInput& match,
                                          const model::PlayerMap& players)
{
    const auto make_team = [&](const model::Team& team)
    {
        return model::Team{.players = {players.at(team.players.first.GetNickname()),
                                       players.at(team.players.second.GetNickname())}};
    };
    const auto make_teams = [&](const model::Teams& teams)
    { return model::Teams{make_team(teams.first), make_team(teams.second)}; };

    return ratings::MatchInput{.teams_         = make_teams(match.teams_),
                               .set_scores_    = match.set_scores_,
                               .team_settings_ = match.team_settings_};
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
    result["teams"]      = names_from_teams(match.teams_);
    result["set_scores"] = match.set_scores_;
    result["team_settings"] =
      nlohmann::json::array({static_cast<std::uint8_t>(match.team_settings_.set1),
                             static_cast<std::uint8_t>(match.team_settings_.set2),
                             static_cast<std::uint8_t>(match.team_settings_.set3),
                             static_cast<std::uint8_t>(match.team_settings_.set4)});
    return result;
}

ratings::MatchInput MatchFromJson(const nlohmann::json& json)
{
    const auto teams{json.at("teams").get<model::TeamNicknames>()};
    const auto settings_array{
      json.at("team_settings").get<std::array<std::uint8_t, model::kSetsPerMatch>>()};
    const auto make_teams = [](const model::TeamNicknames& names)
    {
        return model::Teams{
          model::Team{.players = {Player{names.at(0).first}, Player{names.at(0).second}}},
          model::Team{.players = {Player{names.at(1).first}, Player{names.at(1).second}}}};
    };

    return ratings::MatchInput{
      .teams_         = make_teams(teams),
      .set_scores_    = json.at("set_scores").get<ratings::SetScores>(),
      .team_settings_ = {.set1 = static_cast<model::PlayerPositionRotation>(settings_array.at(0)),
                         .set2 = static_cast<model::PlayerPositionRotation>(settings_array.at(1)),
                         .set3 = static_cast<model::PlayerPositionRotation>(settings_array.at(2)),
                         .set4 = static_cast<model::PlayerPositionRotation>(settings_array.at(3))},
    };
}
}  // namespace convert
