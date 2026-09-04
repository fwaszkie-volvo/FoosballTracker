#include "controller_json.hpp"

#include <nlohmann/detail/json_ref.hpp>
#include <nlohmann/json.hpp>
#include <utility>

namespace controller_json
{
nlohmann::json PlayerJson(const Player& player)
{
    return {{"nickname", player.GetNickname()}, {"elo", player.GetElo()}};
}

nlohmann::json TeamJson(const model::Team& team)
{
    return {{"players", {PlayerJson(team.players.first), PlayerJson(team.players.second)}}};
}

nlohmann::json PositionJson(const model::Team& team, const bool shifted)
{
    const auto& defence_player = shifted ? team.players.second : team.players.first;
    const auto& offence_player = shifted ? team.players.first : team.players.second;
    return {{"defence", defence_player.GetNickname()}, {"offence", offence_player.GetNickname()}};
}

nlohmann::json SettingsJson(const model::Teams& teams, const model::TeamFormation formation)
{
    const bool team_1_shifted = formation == model::TeamFormation::kTeam1Shifted ||
                                formation == model::TeamFormation::kBothShifted;
    const bool team_2_shifted = formation == model::TeamFormation::kTeam2Shifted ||
                                formation == model::TeamFormation::kBothShifted;
    return {{"red", PositionJson(teams.first, team_1_shifted)},
            {"blue", PositionJson(teams.second, team_2_shifted)}};
}
}  // namespace controller_json
