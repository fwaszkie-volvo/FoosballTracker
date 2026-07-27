#include <spdlog/spdlog.h>

#include <filesystem>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>

#include "controller.hpp"
#include "log_level_manager.hpp"
#include "model_main.hpp"
#include "view_main.hpp"

int main(int argc, char* argv[])
{
    const auto& log_level_manager = app_logging::GetLogLevelManager();
    log_level_manager.SetLogLevel();

    const auto executable_path =
      argc > 0 ? std::filesystem::path{argv[0]}.lexically_normal() : std::filesystem::path{};
    spdlog::info("Path: {}", executable_path.string());

    Controller controller{std::make_unique<ModelMain>(), std::make_unique<ViewMain>()};
    return controller.Run();
}
