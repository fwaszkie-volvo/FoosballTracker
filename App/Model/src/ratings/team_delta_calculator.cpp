#include "team_delta_calculator.hpp"

#include <bits/std_abs.h>

#include <algorithm>
#include <array>
#include <cmath>
#include <cstdlib>

namespace ratings
{
int TeamDeltaCalculator::ComputeTeamDelta(const std::size_t team_index,
                                          const MatchInput& match,
                                          const TeamElos& team_elos) const
{
    const auto stats = BuildTeamDeltaStats(team_index, match, team_elos);
    const double expected_score = stats.expected_sum / stats.opponents_count;
    const double actual_score = stats.actual_sum / stats.opponents_count;
    const double multiplier = stats.multiplier_sum / stats.opponents_count;

    return static_cast<int>(
      std::lround(config_.k_factor * multiplier * (actual_score - expected_score)));
}

TeamDeltaCalculator::TeamDeltaStats TeamDeltaCalculator::BuildTeamDeltaStats(
  const std::size_t team_index, const MatchInput& match, const TeamElos& team_elos) const
{
    TeamDeltaStats stats{};
    stats.opponents_count = static_cast<double>((kTeamsNumber - 1U) * kSetsPerMatch);

    for (std::size_t opponent_index{0U}; opponent_index < kTeamsNumber; ++opponent_index)
    {
        if (opponent_index == team_index)
        {
            continue;
        }

        for (std::size_t set_index{0U}; set_index < kSetsPerMatch; ++set_index)
        {
            const auto points_for = match.set_scores.at(team_index).at(set_index);
            const auto points_against = match.set_scores.at(opponent_index).at(set_index);

            stats.expected_sum +=
              ExpectedScore(team_elos.at(team_index), team_elos.at(opponent_index));
            stats.actual_sum += ScoreFromSet(points_for, points_against);
            stats.multiplier_sum += GoalDifferenceMultiplier(points_for, points_against);
        }
    }

    return stats;
}

double TeamDeltaCalculator::ExpectedScore(const double own_elo, const double opponent_elo)
{
    constexpr double kExpectedScoreBase{10.0};
    constexpr double kExpectedScoreScale{400.0};
    constexpr double kWinScore{1.0};

    return kWinScore / (kWinScore + std::pow(kExpectedScoreBase,
                                             (opponent_elo - own_elo) / kExpectedScoreScale));
}

double TeamDeltaCalculator::ScoreFromSet(const std::uint8_t points_for,
                                         const std::uint8_t points_against)
{
    constexpr double kWinScore{1.0};
    constexpr double kLossScore{0.0};
    constexpr double kDrawScore{0.5};

    if (points_for > points_against)
    {
        return kWinScore;
    }

    if (points_for < points_against)
    {
        return kLossScore;
    }

    return kDrawScore;
}

double TeamDeltaCalculator::GoalDifferenceMultiplier(const std::uint8_t points_for,
                                                     const std::uint8_t points_against) const
{
    constexpr double kNoGoalDifferenceBonus{1.0};
    constexpr int kGoalDifferenceNeutralMargin{1};

    if (!config_.use_goal_difference)
    {
        return kNoGoalDifferenceBonus;
    }

    const int goal_difference =
      std::max(0,
               std::abs(static_cast<int>(points_for) - static_cast<int>(points_against)) -
                 kGoalDifferenceNeutralMargin);
    return kNoGoalDifferenceBonus +
           config_.goal_difference_weight * static_cast<double>(goal_difference);
}
}  // namespace ratings
