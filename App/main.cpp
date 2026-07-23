#include <filesystem>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>

#include "controller.hpp"
#include "model_main.hpp"
#include "view_main.hpp"

int main(int argc, char* argv[])
{
    const auto executable_path =
      argc > 0 ? std::filesystem::path{argv[0]}.lexically_normal() : std::filesystem::path{};
    std::cout << "Path: " << executable_path << '\n';

    Controller controller{std::make_unique<ModelMain>(), std::make_unique<ViewMain>()};
    return controller.Run();
}
