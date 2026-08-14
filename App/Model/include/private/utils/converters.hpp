#ifndef FOOSBALL_TRACKER_APP_MODEL_INCLUDE_PRIVATE_UTILS_CONVERTERS_HPP_
#define FOOSBALL_TRACKER_APP_MODEL_INCLUDE_PRIVATE_UTILS_CONVERTERS_HPP_

#include "model_types.hpp"
#include "ratings_types.hpp"

namespace convert
{
model::Team TeamFromNames(const model::NicknamePair& names);
ratings::TeamSettings TeamSettingsFromNames(const model::TeamSettingsNicknames& data);
ratings::MatchInput MatchInputFromPlayers(const ratings::MatchInput& match,
                                          const model::PlayerMap& players);
}  // namespace convert

#endif  // FOOSBALL_TRACKER_APP_MODEL_INCLUDE_PRIVATE_UTILS_CONVERTERS_HPP_
