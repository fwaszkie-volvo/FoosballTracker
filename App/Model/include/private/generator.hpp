#ifndef FOOSBALL_TRACKER_APP_MODEL_INCLUDE_PRIVATE_GENERATOR_HPP_
#define FOOSBALL_TRACKER_APP_MODEL_INCLUDE_PRIVATE_GENERATOR_HPP_

#include <optional>

#include "generator_types.hpp"
#include "model_types.hpp"
#include "ratings_types.hpp"

namespace generator
{
std::optional<model::Teams> GenerateTeamsRandom(const Players& players);
std::optional<model::Teams> GenerateTeamsByElo(const Players& players);
std::optional<model::TeamFormations> GenerateTeamFormationsRandom(const model::Teams& teams);
std::optional<model::TeamFormations> GenerateTeamFormationsStandard(const model::Teams& teams);
}  // namespace generator

#endif  // FOOSBALL_TRACKER_APP_MODEL_INCLUDE_PRIVATE_GENERATOR_HPP_
