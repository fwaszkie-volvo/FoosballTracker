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
std::optional<std::pair<model::Team, model::Team>> GenerateTeamsRandom(const Players& players)
{
    std::array<const Player*, kPlayersCount> shuffled_players{
      &players.at(0), &players.at(1), &players.at(2), &players.at(3)};

    std::mt19937 random_generator{std::random_device{}()};
    std::shuffle(shuffled_players.begin(), shuffled_players.end(), random_generator);

    return std::pair<model::Team, model::Team>{
      model::Team{{*shuffled_players.at(0), *shuffled_players.at(1)}},
      model::Team{{*shuffled_players.at(2), *shuffled_players.at(3)}},
    };
}

std::optional<std::pair<model::Team, model::Team>> GenerateTeamsByElo(const Players& players)
{
    std::array<const Player*, kPlayersCount> players_by_elo{
      &players.at(0), &players.at(1), &players.at(2), &players.at(3)};

    std::ranges::sort(players_by_elo,
                      [](const Player* lhs, const Player* rhs)
                      { return lhs->GetElo() > rhs->GetElo(); });

    return std::pair<model::Team, model::Team>{
      model::Team{{*players_by_elo.front(), *players_by_elo.back()}},
      model::Team{{*players_by_elo.at(1), *players_by_elo.at(2)}},
    };
}
}  // namespace generator
