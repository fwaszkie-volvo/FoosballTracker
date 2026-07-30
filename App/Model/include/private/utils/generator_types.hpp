#ifndef FOOSBALL_TRACKER_APP_MODEL_INCLUDE_PRIVATE_UTILS_GENERATOR_TYPES_HPP_
#define FOOSBALL_TRACKER_APP_MODEL_INCLUDE_PRIVATE_UTILS_GENERATOR_TYPES_HPP_

#include <array>
#include <utility>

#include "model_types.hpp"

namespace generator
{
using TeamDrawResult = std::pair<model::Team, model::Team>;

constexpr std::size_t kPlayersCount{4};
using Players = std::array<Player, generator::kPlayersCount>;
}  // namespace generator

#endif  // FOOSBALL_TRACKER_APP_MODEL_INCLUDE_PRIVATE_UTILS_GENERATOR_TYPES_HPP_
