#include "match_elo_calculator.hpp"

#include <bits/std_abs.h>

#include <algorithm>
#include <array>
#include <cmath>
#include <cstdint>
#include <cstdlib>

namespace
{
model::TeamScores<double> BuildTeamElos(const ratings::MatchInput& match)
{
    model::TeamScores<double> team_elos{};

    for (std::size_t team_index{0U}; team_index < model::kTeamsNumber; ++team_index)
    {
        team_elos.at(team_index) = match.teams_.at(team_index).GetAverageElo();
    }

    return team_elos;
}

double ExpectedScore(const double own_elo, const double opponent_elo)
{
    constexpr double kExpectedScoreBase{10.0};
    constexpr double kExpectedScoreScale{400.0};
    constexpr double kWinScore{1.0};

    return kWinScore / (kWinScore + std::pow(kExpectedScoreBase,
                                             (opponent_elo - own_elo) / kExpectedScoreScale));
}

double ScoreFromSet(const std::uint8_t points_for, const std::uint8_t points_against)
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

double GoalDifferenceMultiplier(const std::uint8_t points_for, const std::uint8_t points_against)
{
    static constexpr double kNoGoalDifferenceBonus{1.0};
    static constexpr int kGoalDifferenceNeutralMargin{1};
    static constexpr double kGoalDifferenceWeight{0.1};

    const int goal_difference =
      std::max(0,
               std::abs(static_cast<int>(points_for) - static_cast<int>(points_against)) -
                 kGoalDifferenceNeutralMargin);
    return kNoGoalDifferenceBonus + kGoalDifferenceWeight * static_cast<double>(goal_difference);
}

double ComputeExpectedScore(const std::size_t team_index,
                            const ratings::MatchInput& match,
                            const model::TeamScores<double>& team_elos)
{
    const double own_elo = team_elos.at(team_index);
    double expected_sum{0.0};

    for (std::size_t opponent_index{0U}; opponent_index < model::kTeamsNumber; ++opponent_index)
    {
        if (opponent_index == team_index)
        {
            continue;
        }

        const double opponent_elo = team_elos.at(opponent_index);

        for (std::size_t set_index{0U}; set_index < model::kSetsPerMatch; ++set_index)
        {
            expected_sum += ExpectedScore(own_elo, opponent_elo);
        }
    }

    return expected_sum;
}

double ComputeActualScore(const std::size_t team_index, const ratings::MatchInput& match)
{
    double actual_sum{0.0};

    for (std::size_t opponent_index{0U}; opponent_index < model::kTeamsNumber; ++opponent_index)
    {
        if (opponent_index == team_index)
        {
            continue;
        }

        for (std::size_t set_index{0U}; set_index < model::kSetsPerMatch; ++set_index)
        {
            const auto points_for = match.set_scores_.at(team_index).at(set_index);
            const auto points_against = match.set_scores_.at(opponent_index).at(set_index);
            actual_sum += ScoreFromSet(points_for, points_against);
        }
    }

    return actual_sum;
}

double ComputeMultiplier(const std::size_t team_index, const ratings::MatchInput& match)
{
    double multiplier_sum{0.0};

    for (std::size_t opponent_index{0U}; opponent_index < model::kTeamsNumber; ++opponent_index)
    {
        if (opponent_index == team_index)
        {
            continue;
        }

        for (std::size_t set_index{0U}; set_index < model::kSetsPerMatch; ++set_index)
        {
            const auto points_for = match.set_scores_.at(team_index).at(set_index);
            const auto points_against = match.set_scores_.at(opponent_index).at(set_index);
            multiplier_sum += GoalDifferenceMultiplier(points_for, points_against);
        }
    }

    return multiplier_sum;
}
}  // namespace

namespace calculator
{
model::TeamScores<int> ComputeMatchElos(const ratings::MatchInput& match)
{
    const model::TeamScores<double> team_elos = BuildTeamElos(match);
    model::TeamScores<int> deltas{};

    static constexpr double kFactor{32.0};

    for (std::size_t team_index{0U}; team_index < model::kTeamsNumber; ++team_index)
    {
        const double expected_score = ComputeExpectedScore(team_index, match, team_elos) /
                                      static_cast<double>(model::kSetsPerMatch);
        const double actual_score =
          ComputeActualScore(team_index, match) / static_cast<double>(model::kSetsPerMatch);
        const double multiplier =
          ComputeMultiplier(team_index, match) / static_cast<double>(model::kSetsPerMatch);

        deltas.at(team_index) =
          static_cast<int>(std::lround(kFactor * multiplier * (actual_score - expected_score)));
    }

    return deltas;
}
}  // namespace calculator
