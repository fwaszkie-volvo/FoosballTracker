#ifndef FOOSBALL_TRACKER_APP_MODEL_INCLUDE_PRIVATE_UTILS_MODEL_TYPES_HPP_
#define FOOSBALL_TRACKER_APP_MODEL_INCLUDE_PRIVATE_UTILS_MODEL_TYPES_HPP_

#include <array>
#include <cstddef>
#include <cstdint>
#include <string>
#include <unordered_map>
#include <utility>

#include "player.hpp"

namespace model
{
constexpr std::size_t kTeamSize{2};
constexpr std::size_t kTeamsNumber{2};
constexpr std::size_t kSetsPerMatch{4};

enum class TeamFormation : std::uint8_t
{
    kStandard,
    kTeam1Shifted,
    kTeam2Shifted,
    kBothShifted
};

using TeamFormations = std::array<TeamFormation, kSetsPerMatch>;
using Nickname       = std::string;

class Team final
{
  public:
    std::pair<Player, Player> players;

    double GetAverageElo() const
    {
        return (static_cast<double>(players.first.GetElo()) +
                static_cast<double>(players.second.GetElo())) /
               static_cast<double>(kTeamSize);
    }
};

using Teams         = std::pair<model::Team, model::Team>;
using TeamSetScores = std::array<std::uint8_t, kSetsPerMatch>;
using PlayerMap     = std::unordered_map<Nickname, Player>;
using PlayerEloMap  = std::unordered_map<Nickname, int>;
}  // namespace model

#endif  // FOOSBALL_TRACKER_APP_MODEL_INCLUDE_PRIVATE_UTILS_MODEL_TYPES_HPP_
