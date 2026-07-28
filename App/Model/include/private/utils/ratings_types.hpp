#ifndef FOOSBALL_TRACKER_APP_MODEL_INCLUDE_PRIVATE_UTILS_RATINGS_TYPES_HPP_
#define FOOSBALL_TRACKER_APP_MODEL_INCLUDE_PRIVATE_UTILS_RATINGS_TYPES_HPP_

#include <array>
#include <cstdint>
#include <string>

namespace ratings
{
constexpr int kDefaultElo{1000};
constexpr std::size_t kTeamSize{2};
constexpr std::size_t kTeamsNumber{2};
constexpr std::size_t kSetsPerMatch{4};

using Nickname = std::string;
using TeamNicknames = std::array<Nickname, kTeamSize>;
using PlayersNicknames = std::array<TeamNicknames, kTeamsNumber>;
using TeamElos = std::array<double, kTeamsNumber>;
using TeamSetScores = std::array<std::uint8_t, kSetsPerMatch>;
using MatchSetScores = std::array<TeamSetScores, kTeamsNumber>;

struct EloConfig final
{
    int initial_rating{kDefaultElo};
    double k_factor{32.0};
    bool use_goal_difference{};
    double goal_difference_weight{};
};

struct MatchInput final
{
    PlayersNicknames nicknames;
    MatchSetScores set_scores{};
};
}  // namespace ratings

#endif  // FOOSBALL_TRACKER_APP_MODEL_INCLUDE_PRIVATE_UTILS_RATINGS_TYPES_HPP_
