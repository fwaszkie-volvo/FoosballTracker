#ifndef FOOSBALL_TRACKER_APP_MODEL_INCLUDE_PRIVATE_UTILS_RATINGS_TYPES_HPP_
#define FOOSBALL_TRACKER_APP_MODEL_INCLUDE_PRIVATE_UTILS_RATINGS_TYPES_HPP_

#include <utility>

#include "model_types.hpp"

namespace ratings
{
struct SetAverages final
{
    double set_score{};
    double goal_margin_multiplier{};
};

using SetScores = std::array<model::TeamSetScores, model::kTeamsNumber>;

struct MatchInput final
{
    std::pair<model::Team, model::Team> teams_;
    SetScores set_scores_{};
};
}  // namespace ratings

#endif  // FOOSBALL_TRACKER_APP_MODEL_INCLUDE_PRIVATE_UTILS_RATINGS_TYPES_HPP_
