#include "generator.hpp"

#include <algorithm>
#include <array>
#include <cstddef>
#include <numeric>
#include <optional>
#include <random>
#include <utility>
#include <vector>

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

std::optional<ratings::TeamSettings> GenerateTeamSettingsRandom(const model::Teams& teams)
{
    std::array<const Player*, kPlayersCount> slot_players{&teams.first.players.first,
                                                          &teams.first.players.second,
                                                          &teams.second.players.first,
                                                          &teams.second.players.second};

    std::vector<model::Teams> all_settings{};
    std::ranges::sort(slot_players);
    do
    {
        all_settings.emplace_back(model::Team{{*slot_players.at(0), *slot_players.at(1)}},
                                  model::Team{{*slot_players.at(2), *slot_players.at(3)}});
    } while (std::ranges::next_permutation(slot_players).found);

    std::vector<std::size_t> setting_indices(all_settings.size());
    std::iota(setting_indices.begin(), setting_indices.end(), static_cast<std::size_t>(0U));

    std::mt19937 random_generator{std::random_device{}()};
    std::ranges::shuffle(setting_indices, random_generator);

    return ratings::TeamSettings{
      all_settings.at(setting_indices.at(0)),
      all_settings.at(setting_indices.at(1)),
      all_settings.at(setting_indices.at(2)),
      all_settings.at(setting_indices.at(3)),
    };
}

std::optional<ratings::TeamSettings> GenerateTeamSettingsStandard(const model::Teams& teams)
{
    const model::Teams& first_set{teams};

    const Player& first_team_first_player   = first_set.first.players.first;
    const Player& first_team_second_player  = first_set.first.players.second;
    const Player& second_team_first_player  = first_set.second.players.first;
    const Player& second_team_second_player = first_set.second.players.second;

    ratings::TeamSettings settings{
      first_set,
      model::Teams{model::Team{{first_team_first_player, second_team_first_player}},
                   model::Team{{first_team_second_player, second_team_second_player}}},
      model::Teams{model::Team{{second_team_first_player, first_team_first_player}},
                   model::Team{{first_team_second_player, second_team_second_player}}},
      model::Teams{model::Team{{second_team_first_player, first_team_first_player}},
                   model::Team{{second_team_second_player, first_team_second_player}}},
    };

    return settings;
}
}  // namespace generator
