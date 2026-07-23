#include "generator.hpp"

#include <algorithm>
#include <optional>
#include <random>
#include <string>
#include <vector>

namespace generator
{
std::optional<TeamDraw> GenerateTeams(const Players& nicknames)
{
    std::vector<std::string> shuffled_nicknames{nicknames.begin(), nicknames.end()};

    std::mt19937 random_generator{std::random_device{}()};
    std::shuffle(shuffled_nicknames.begin(), shuffled_nicknames.end(), random_generator);

    TeamDraw team_draw{
      .first_team =
        Team{.player_one = shuffled_nicknames.at(0), .player_two = shuffled_nicknames.at(1)},
      .second_team =
        Team{.player_one = shuffled_nicknames.at(2), .player_two = shuffled_nicknames.at(3)},
    };

    return team_draw;
}
}  // namespace generator
