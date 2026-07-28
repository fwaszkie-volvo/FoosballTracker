#ifndef FOOSBALL_TRACKER_APP_MODEL_INCLUDE_PRIVATE_RATINGS_TEAM_DELTA_CALCULATOR_HPP_
#define FOOSBALL_TRACKER_APP_MODEL_INCLUDE_PRIVATE_RATINGS_TEAM_DELTA_CALCULATOR_HPP_

#include <cstddef>
#include <cstdint>

#include "ratings_types.hpp"

namespace ratings
{
class TeamDeltaCalculator
{
  public:
    TeamDeltaCalculator(const EloConfig& config) : config_(config) {}

    int ComputeTeamDelta(std::size_t team_index,
                         const MatchInput& match,
                         const TeamElos& team_elos) const;

  private:
    struct TeamDeltaStats final
    {
        double expected_sum{};
        double actual_sum{};
        double multiplier_sum{};
        double opponents_count{};
    };

    TeamDeltaStats BuildTeamDeltaStats(std::size_t team_index,
                                       const MatchInput& match,
                                       const TeamElos& team_elos) const;

    static double ExpectedScore(double own_elo, double opponent_elo);
    static double ScoreFromSet(std::uint8_t points_for, std::uint8_t points_against);
    double GoalDifferenceMultiplier(std::uint8_t points_for, std::uint8_t points_against) const;

    EloConfig config_{};
};
}  // namespace ratings

#endif  // FOOSBALL_TRACKER_APP_MODEL_INCLUDE_PRIVATE_RATINGS_TEAM_DELTA_CALCULATOR_HPP_
