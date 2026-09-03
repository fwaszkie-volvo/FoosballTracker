#ifndef FOOSBALL_TRACKER_APP_MODEL_INCLUDE_PRIVATE_UTILS_CONVERTERS_HPP_
#define FOOSBALL_TRACKER_APP_MODEL_INCLUDE_PRIVATE_UTILS_CONVERTERS_HPP_

#include <nlohmann/json_fwd.hpp>

#include "model_types.hpp"
#include "ratings_types.hpp"

namespace convert
{
ratings::MatchInput MatchInputFromPlayers(const ratings::MatchInput& match,
                                          const model::PlayerMap& players);

nlohmann::json MatchToJson(const ratings::MatchInput& match);
ratings::MatchInput MatchFromJson(const nlohmann::json& json);
}  // namespace convert

#endif  // FOOSBALL_TRACKER_APP_MODEL_INCLUDE_PRIVATE_UTILS_CONVERTERS_HPP_
