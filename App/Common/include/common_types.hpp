#ifndef FOOSBALL_TRACKER_APP_COMMON_INCLUDE_COMMON_TYPES_HPP_
#define FOOSBALL_TRACKER_APP_COMMON_INCLUDE_COMMON_TYPES_HPP_

#include <string>
#include <utility>

namespace common
{
using Nickname = std::string;

// HTTP status code paired with a JSON-encoded response body.
using HttpResult = std::pair<int, std::string>;
}  // namespace common

#endif  // FOOSBALL_TRACKER_APP_COMMON_INCLUDE_COMMON_TYPES_HPP_
