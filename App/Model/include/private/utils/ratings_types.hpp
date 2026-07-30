#ifndef FOOSBALL_TRACKER_APP_MODEL_INCLUDE_PRIVATE_UTILS_RATINGS_TYPES_HPP_
#define FOOSBALL_TRACKER_APP_MODEL_INCLUDE_PRIVATE_UTILS_RATINGS_TYPES_HPP_

#include <array>

#include "model_types.hpp"

namespace ratings
{
template <typename T>
using TeamArray = std::array<T, model::kTeamsNumber>;

struct MatchInput final
{
    TeamArray<model::Team> teams_;
    TeamArray<model::TeamSetScores> set_scores_{};
};
}  // namespace ratings

#endif  // FOOSBALL_TRACKER_APP_MODEL_INCLUDE_PRIVATE_UTILS_RATINGS_TYPES_HPP_
