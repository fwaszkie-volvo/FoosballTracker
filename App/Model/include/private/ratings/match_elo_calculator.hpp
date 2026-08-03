#ifndef FOOSBALL_TRACKER_APP_MODEL_INCLUDE_PRIVATE_RATINGS_MATCH_ELO_CALCULATOR_HPP_
#define FOOSBALL_TRACKER_APP_MODEL_INCLUDE_PRIVATE_RATINGS_MATCH_ELO_CALCULATOR_HPP_

#include "ratings_types.hpp"

namespace calculator
{
int ComputeFirstTeamEloDelta(const ratings::MatchInput& match);
}  // namespace calculator

#endif  // FOOSBALL_TRACKER_APP_MODEL_INCLUDE_PRIVATE_RATINGS_MATCH_ELO_CALCULATOR_HPP_
