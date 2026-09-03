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
std::optional<model::Teams> GenerateTeamsRandom(const Players& players)
{
    std::array<const Player*, kPlayersCount> shuffled_players{
      &players.at(0), &players.at(1), &players.at(2), &players.at(3)};

    std::mt19937 random_generator{std::random_device{}()};
    std::shuffle(shuffled_players.begin(), shuffled_players.end(), random_generator);

    return model::Teams{
      model::Team{{*shuffled_players.at(0), *shuffled_players.at(1)}},
      model::Team{{*shuffled_players.at(2), *shuffled_players.at(3)}},
    };
}

std::optional<model::Teams> GenerateTeamsByElo(const Players& players)
{
    std::array<const Player*, kPlayersCount> players_by_elo{
      &players.at(0), &players.at(1), &players.at(2), &players.at(3)};

    std::ranges::sort(players_by_elo,
                      [](const Player* lhs, const Player* rhs)
                      { return lhs->GetElo() > rhs->GetElo(); });

    return model::Teams{
      model::Team{{*players_by_elo.front(), *players_by_elo.back()}},
      model::Team{{*players_by_elo.at(1), *players_by_elo.at(2)}},
    };
}

std::optional<model::TeamFormations> GenerateTeamSettingsRandom(const model::Teams& teams)
{
    std::array<model::TeamFormation, model::kSetsPerMatch> all_settings{
      model::TeamFormation::kStandard,
      model::TeamFormation::kTeam1Shifted,
      model::TeamFormation::kTeam2Shifted,
      model::TeamFormation::kBothShifted};

    std::mt19937 random_generator{std::random_device{}()};
    std::ranges::shuffle(all_settings.begin(), all_settings.end(), random_generator);

    return all_settings;
}

std::optional<model::TeamFormations> GenerateTeamSettingsStandard(const model::Teams& teams)
{
    return model::TeamFormations{
      model::TeamFormation::kStandard,
      model::TeamFormation::kTeam2Shifted,
      model::TeamFormation::kTeam1Shifted,
      model::TeamFormation::kBothShifted,
    };
}
}  // namespace generator
