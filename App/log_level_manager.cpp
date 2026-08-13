#include "log_level_manager.hpp"

#include <spdlog/logger.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>

#include <cstdlib>
#include <memory>
#include <utility>

namespace app_logging
{
void LogLevelManager::SetLogLevel() const
{
    const auto resolved_level  = Resolve(std::getenv("LOG_LEVEL"));
    constexpr auto kLogPattern = "[%H:%M:%S.%e] [%^%l%$] %v";

    auto sink   = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
    auto logger = std::make_shared<spdlog::logger>("foosball_tracker_app", sink);
    spdlog::set_default_logger(logger);

    auto* default_logger = spdlog::default_logger_raw();
    default_logger->set_level(resolved_level.level);
    default_logger->set_pattern(kLogPattern);
}

LogLevelManager::ResolvedLevel LogLevelManager::Resolve(const char* level_env) const
{
    const auto level_name = std::string_view(level_env ? level_env : "");
    const auto it         = levels_.find(level_name);
    return (it == levels_.end()) ? ResolvedLevel{.level = default_level_, .label = default_label_}
                                 : ResolvedLevel{.level = it->second, .label = level_name};
}

const LogLevelManager& GetLogLevelManager()
{
    static const LogLevelManager level_manager;
    return level_manager;
}
}  // namespace app_logging
