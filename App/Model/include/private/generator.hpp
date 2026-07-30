#ifndef FOOSBALL_TRACKER_APP_MODEL_INCLUDE_PRIVATE_GENERATOR_HPP_
#define FOOSBALL_TRACKER_APP_MODEL_INCLUDE_PRIVATE_GENERATOR_HPP_

#include <optional>
#include <utility>

#include "generator_types.hpp"

namespace generator
{
std::optional<std::pair<model::Team, model::Team>> GenerateTeamsRandom(const Players& players);
std::optional<std::pair<model::Team, model::Team>> GenerateTeamsByElo(const Players& players);
}  // namespace generator

#endif  // FOOSBALL_TRACKER_APP_MODEL_INCLUDE_PRIVATE_GENERATOR_HPP_
