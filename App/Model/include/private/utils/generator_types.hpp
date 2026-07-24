#ifndef FOOSBALL_TRACKER_APP_MODEL_INCLUDE_PRIVATE_UTILS_GENERATOR_TYPES_HPP_
#define FOOSBALL_TRACKER_APP_MODEL_INCLUDE_PRIVATE_UTILS_GENERATOR_TYPES_HPP_

#include <array>
#include <optional>
#include <string>

#include "player.hpp"

namespace generator
{
struct Team final
{
    Player first_player;
    Player second_player;
};

struct TeamDrawResult final
{
    Team first_team;
    Team second_team;
};

constexpr std::size_t kPlayersCount{4};

using Players = std::array<Player, generator::kPlayersCount>;
}  // namespace generator

#endif  // FOOSBALL_TRACKER_APP_MODEL_INCLUDE_PRIVATE_UTILS_GENERATOR_TYPES_HPP_
