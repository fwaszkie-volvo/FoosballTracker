#include "generator.hpp"

#include <algorithm>
#include <array>
#include <optional>
#include <random>
#include <utility>

#include "model_types.hpp"
#include "player.hpp"

namespace generator
{
std::optional<TeamDrawResult> GenerateTeams(const Players& players)
{
    std::mt19937 random_generator{std::random_device{}()};

    std::array<const Player*, kPlayersCount> shuffled_players{
      &players.at(0), &players.at(1), &players.at(2), &players.at(3)};
    std::shuffle(shuffled_players.begin(), shuffled_players.end(), random_generator);

    return TeamDrawResult{
      model::Team{{*shuffled_players.at(0), *shuffled_players.at(1)}},
      model::Team{{*shuffled_players.at(2), *shuffled_players.at(3)}},
    };
}

}  // namespace generator
