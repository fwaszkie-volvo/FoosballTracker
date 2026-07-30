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
    std::array<const Player*, kPlayersCount> players_by_elo{
      &players.at(0), &players.at(1), &players.at(2), &players.at(3)};

    std::mt19937 random_generator{std::random_device{}()};
    std::shuffle(players_by_elo.begin(), players_by_elo.end(), random_generator);

    std::ranges::sort(players_by_elo,
                      [](const Player* lhs, const Player* rhs)
                      { return lhs->GetElo() > rhs->GetElo(); });

    return TeamDrawResult{
      model::Team{{*players_by_elo.front(), *players_by_elo.back()}},
      model::Team{{*players_by_elo.at(1), *players_by_elo.at(2)}},
    };
}
}  // namespace generator
