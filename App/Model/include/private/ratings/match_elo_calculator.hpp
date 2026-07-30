#ifndef FOOSBALL_TRACKER_APP_MODEL_INCLUDE_PRIVATE_RATINGS_MATCH_ELO_CALCULATOR_HPP_
#define FOOSBALL_TRACKER_APP_MODEL_INCLUDE_PRIVATE_RATINGS_MATCH_ELO_CALCULATOR_HPP_

#include "model_types.hpp"
#include "ratings_types.hpp"

namespace calculator
{
model::TeamsArray<int> ComputeMatchElos(const ratings::MatchInput& match);
}  // namespace calculator

#endif  // FOOSBALL_TRACKER_APP_MODEL_INCLUDE_PRIVATE_RATINGS_MATCH_ELO_CALCULATOR_HPP_
