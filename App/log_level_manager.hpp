#ifndef FOOSBALL_TRACKER_APP_LOG_LEVEL_MANAGER_HPP_
#define FOOSBALL_TRACKER_APP_LOG_LEVEL_MANAGER_HPP_

#include <spdlog/common.h>

#include <string_view>
#include <unordered_map>
#include <utility>

namespace app_logging
{
class LogLevelManager
{
  public:
    struct ResolvedLevel
    {
        spdlog::level::level_enum level;
        std::string_view label;
    };

    void SetLogLevel() const;
    ResolvedLevel Resolve(const char* level_env) const;

  private:
    const std::unordered_map<std::string_view, spdlog::level::level_enum> levels_{
      {"trace", spdlog::level::trace},
      {"debug", spdlog::level::debug},
      {"info", spdlog::level::info},
      {"warn", spdlog::level::warn},
      {"warning", spdlog::level::warn},
      {"err", spdlog::level::err},
      {"error", spdlog::level::err},
      {"critical", spdlog::level::critical},
      {"off", spdlog::level::off},
    };

    const spdlog::level::level_enum default_level_{spdlog::level::info};
    static constexpr std::string_view default_label_{"info (default)"};
};

const LogLevelManager& GetLogLevelManager();

}  // namespace app_logging

#endif  // FOOSBALL_TRACKER_APP_LOG_LEVEL_MANAGER_HPP_
