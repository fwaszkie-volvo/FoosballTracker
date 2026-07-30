#ifndef FOOSBALL_TRACKER_APP_MODEL_INCLUDE_PRIVATE_UTILS_MODEL_TYPES_HPP_
#define FOOSBALL_TRACKER_APP_MODEL_INCLUDE_PRIVATE_UTILS_MODEL_TYPES_HPP_

#include <array>
#include <cstddef>
#include <cstdint>
#include <string>
#include <utility>

#include "player.hpp"

namespace model
{
constexpr std::size_t kTeamSize{2};
constexpr std::size_t kTeamsNumber{2};
constexpr std::size_t kSetsPerMatch{4};

using Nickname = std::string;

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

template <typename T>
using TeamsArray = std::array<T, kTeamsNumber>;

using TeamSetScores = std::array<std::uint8_t, kSetsPerMatch>;
}  // namespace model

#endif  // FOOSBALL_TRACKER_APP_MODEL_INCLUDE_PRIVATE_UTILS_MODEL_TYPES_HPP_
