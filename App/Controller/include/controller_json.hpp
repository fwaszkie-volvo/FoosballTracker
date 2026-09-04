#ifndef FOOSBALL_TRACKER_APP_CONTROLLER_INCLUDE_CONTROLLER_JSON_HPP_
#define FOOSBALL_TRACKER_APP_CONTROLLER_INCLUDE_CONTROLLER_JSON_HPP_

#include <nlohmann/json_fwd.hpp>

#include "model_types.hpp"
#include "player.hpp"

namespace controller_json
{
nlohmann::json PlayerJson(const Player& player);
nlohmann::json TeamJson(const model::Team& team);
nlohmann::json PositionJson(const model::Team& team, bool shifted);
nlohmann::json SettingsJson(const model::Teams& teams, model::TeamFormation formation);
}  // namespace controller_json

#endif  // FOOSBALL_TRACKER_APP_CONTROLLER_INCLUDE_CONTROLLER_JSON_HPP_
