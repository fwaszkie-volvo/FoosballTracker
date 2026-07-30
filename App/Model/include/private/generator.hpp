#ifndef FOOSBALL_TRACKER_APP_MODEL_INCLUDE_PRIVATE_GENERATOR_HPP_
#define FOOSBALL_TRACKER_APP_MODEL_INCLUDE_PRIVATE_GENERATOR_HPP_

#include <optional>

#include "generator_types.hpp"

namespace generator
{
std::optional<TeamDrawResult> GenerateTeamsRandom(const Players& players);
std::optional<TeamDrawResult> GenerateTeamsByElo(const Players& players);
}  // namespace generator

#endif  // FOOSBALL_TRACKER_APP_MODEL_INCLUDE_PRIVATE_GENERATOR_HPP_
