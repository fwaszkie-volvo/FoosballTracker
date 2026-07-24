#include "generator.hpp"

#include <algorithm>
#include <optional>
#include <random>
#include <vector>

#include "player.hpp"

namespace generator
{
std::optional<TeamDrawResult> GenerateTeams(const Players& players)
{
    std::vector<Player> shuffled_players{players.begin(), players.end()};

    std::mt19937 random_generator{std::random_device{}()};
    std::shuffle(shuffled_players.begin(), shuffled_players.end(), random_generator);

    TeamDrawResult team_draw{
      .first_team =
        Team{.first_player = shuffled_players.at(0), .second_player = shuffled_players.at(1)},
      .second_team =
        Team{.first_player = shuffled_players.at(2), .second_player = shuffled_players.at(3)},
    };

    return team_draw;
}
}  // namespace generator
