#include "match_elo_calculator.hpp"

#include <bits/std_abs.h>

#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <utility>

#include "model_types.hpp"

namespace
{
constexpr double kWinScore{1.0};

double ExpectedScore(const double own_elo, const double opponent_elo)
{
    constexpr double kExpectedScoreBase{10.0};
    constexpr double kExpectedScoreScale{400.0};

    return kWinScore / (kWinScore + std::pow(kExpectedScoreBase,
                                             (opponent_elo - own_elo) / kExpectedScoreScale));
}

double ComputeSetScore(const int signed_goal_difference)
{
    static constexpr double kLossScore{0.0};
    static constexpr double kDrawScore{0.5};

    if (signed_goal_difference > 0)
    {
        return kWinScore;
    }

    if (signed_goal_difference < 0)
    {
        return kLossScore;
    }

    return kDrawScore;
}

double ComputeGoalMarginMultiplier(const int signed_goal_difference)
{
    static constexpr double kNoGoalDifferenceBonus{1.0};
    static constexpr int kGoalDifferenceNeutralMargin{1};
    static constexpr double kGoalDifferenceWeight{0.1};

    const int goal_difference =
      std::max(0, std::abs(signed_goal_difference) - kGoalDifferenceNeutralMargin);
    return kNoGoalDifferenceBonus + kGoalDifferenceWeight * static_cast<double>(goal_difference);
}

ratings::SetAverages ComputeAverageSetScoreAndMultiplier(const model::TeamSetScores& points_for,
                                                         const model::TeamSetScores& points_against)
{
    double total_set_score{0.0};
    double total_goal_margin_multiplier{0.0};

    for (std::size_t set_index{0U}; set_index < model::kSetsPerMatch; ++set_index)
    {
        const int signed_goal_difference = static_cast<int>(points_for.at(set_index)) -
                                           static_cast<int>(points_against.at(set_index));

        total_set_score += ComputeSetScore(signed_goal_difference);
        total_goal_margin_multiplier += ComputeGoalMarginMultiplier(signed_goal_difference);
    }

    return ratings::SetAverages{
      .set_score = total_set_score / static_cast<double>(model::kSetsPerMatch),
      .goal_margin_multiplier =
        total_goal_margin_multiplier / static_cast<double>(model::kSetsPerMatch),
    };
}
}  // namespace

namespace calculator
{
int ComputeFirstTeamEloDelta(const ratings::MatchInput& match)
{
    static constexpr double kFactor{32.0};
    static constexpr std::size_t kFirstTeamIndex{0U};
    static constexpr std::size_t kSecondTeamIndex{1U};

    const double first_team_elo = match.teams_.first.GetAverageElo();
    const double second_team_elo = match.teams_.second.GetAverageElo();

    const double expected_first_team = ExpectedScore(first_team_elo, second_team_elo);
    const ratings::SetAverages first_team_averages = ComputeAverageSetScoreAndMultiplier(
      match.set_scores_.at(kFirstTeamIndex), match.set_scores_.at(kSecondTeamIndex));

    return static_cast<int>(std::round(kFactor * first_team_averages.goal_margin_multiplier *
                                       (first_team_averages.set_score - expected_first_team)));
}
}  // namespace calculator
