#ifndef FOOSBALL_TRACKER_APP_MODEL_INCLUDE_PRIVATE_UTILS_RATINGS_TYPES_HPP_
#define FOOSBALL_TRACKER_APP_MODEL_INCLUDE_PRIVATE_UTILS_RATINGS_TYPES_HPP_

#include "model_types.hpp"

namespace ratings
{
struct SetAverages final
{
    double set_score{};
    double goal_margin_multiplier{};
};

struct MatchInput final
{
    model::TeamsArray<model::Team> teams_;
    model::TeamsArray<model::TeamSetScores> set_scores_{};
};
}  // namespace ratings

#endif  // FOOSBALL_TRACKER_APP_MODEL_INCLUDE_PRIVATE_UTILS_RATINGS_TYPES_HPP_
