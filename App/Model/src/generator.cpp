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

std::optional<model::TeamSettings> GenerateTeamSettingsRandom(const model::Teams& teams)
{
    std::array<model::PlayerPositionRotation, model::kSetsPerMatch> all_settings{
      model::PlayerPositionRotation::None,
      model::PlayerPositionRotation::Team1Rotated,
      model::PlayerPositionRotation::Team2Rotated,
      model::PlayerPositionRotation::BothRotated};

    std::mt19937 random_generator{std::random_device{}()};
    std::ranges::shuffle(all_settings.begin(), all_settings.end(), random_generator);

    return model::TeamSettings{
      .set1 = all_settings.at(0),
      .set2 = all_settings.at(1),
      .set3 = all_settings.at(2),
      .set4 = all_settings.at(3),
    };
}

std::optional<model::TeamSettings> GenerateTeamSettingsStandard(const model::Teams& teams)
{
    return model::TeamSettings{
      .set1 = model::PlayerPositionRotation::None,
      .set2 = model::PlayerPositionRotation::Team2Rotated,
      .set3 = model::PlayerPositionRotation::Team1Rotated,
      .set4 = model::PlayerPositionRotation::BothRotated,
    };
}
}  // namespace generator
