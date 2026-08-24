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

enum class PlayerPositionRotation : std::uint8_t
{
    None,
    Team1Rotated,
    Team2Rotated,
    BothRotated
};

struct TeamSettings final
{
    PlayerPositionRotation set1;
    PlayerPositionRotation set2;
    PlayerPositionRotation set3;
    PlayerPositionRotation set4;
};

using Nickname              = std::string;
using NicknamePair          = std::pair<Nickname, Nickname>;
using TeamNicknames         = std::array<NicknamePair, kTeamsNumber>;
using TeamSettingsNicknames = std::array<TeamNicknames, kSetsPerMatch>;

struct Team final
{
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
