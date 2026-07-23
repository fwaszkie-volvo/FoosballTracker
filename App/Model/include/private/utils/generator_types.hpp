#ifndef FOOSBALL_TRACKER_APP_MODEL_INCLUDE_PRIVATE_UTILS_GENERATOR_TYPES_HPP_
#define FOOSBALL_TRACKER_APP_MODEL_INCLUDE_PRIVATE_UTILS_GENERATOR_TYPES_HPP_

#include <array>
#include <optional>
#include <string>

namespace generator
{
struct Team final
{
    std::string player_one;
    std::string player_two;
};

struct TeamDraw final
{
    Team first_team;
    Team second_team;
};

constexpr std::size_t kPlayersCount{4};

using Players = std::array<std::string, generator::kPlayersCount>;
}  // namespace generator

#endif  // FOOSBALL_TRACKER_APP_MODEL_INCLUDE_PRIVATE_UTILS_GENERATOR_TYPES_HPP_
