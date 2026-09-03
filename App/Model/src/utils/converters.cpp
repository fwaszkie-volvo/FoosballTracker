#include "converters.hpp"

#include <algorithm>
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
    std::array<std::uint8_t, model::kSetsPerMatch> formations{};
    std::ranges::transform(match.team_settings_,
                           formations.begin(),
                           [](const model::TeamFormation formation)
                           { return static_cast<std::uint8_t>(formation); });

    nlohmann::json result{};
    result["teams"] = nlohmann::json::array(
      {nlohmann::json::array({match.teams_.first.players.first.GetNickname(),
                              match.teams_.first.players.second.GetNickname()}),
       nlohmann::json::array({match.teams_.second.players.first.GetNickname(),
                              match.teams_.second.players.second.GetNickname()})});
    result["set_scores"]    = match.set_scores_;
    result["team_settings"] = formations;
    return result;
}

ratings::MatchInput MatchFromJson(const nlohmann::json& json)
{
    const auto& teams_json{json.at("teams")};
    const model::Teams teams{
      model::Team{.players = {Player{teams_json.at(0).at(0).get<model::Nickname>()},
                              Player{teams_json.at(0).at(1).get<model::Nickname>()}}},
      model::Team{.players = {Player{teams_json.at(1).at(0).get<model::Nickname>()},
                              Player{teams_json.at(1).at(1).get<model::Nickname>()}}}};
    const auto serialized_formations{
      json.at("team_settings").get<std::array<std::uint8_t, model::kSetsPerMatch>>()};
    model::TeamFormations formations{};
    std::ranges::transform(serialized_formations,
                           formations.begin(),
                           [](const std::uint8_t formation)
                           { return static_cast<model::TeamFormation>(formation); });

    return ratings::MatchInput{
      .teams_         = teams,
      .set_scores_    = json.at("set_scores").get<ratings::SetScores>(),
      .team_settings_ = formations,
    };
}
}  // namespace convert
