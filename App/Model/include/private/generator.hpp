#ifndef FOOSBALL_TRACKER_APP_MODEL_INCLUDE_PRIVATE_GENERATOR_HPP_
#define FOOSBALL_TRACKER_APP_MODEL_INCLUDE_PRIVATE_GENERATOR_HPP_

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

std::optional<TeamDraw> GenerateTeams(const std::array<std::string, kPlayersCount>& nicknames);
}  // namespace generator

#endif  // FOOSBALL_TRACKER_APP_MODEL_INCLUDE_PRIVATE_GENERATOR_HPP_
