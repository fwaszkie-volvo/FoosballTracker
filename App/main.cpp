#include <iostream>

#include "controller.hpp"
#include "model_main.hpp"
#include "view_main.hpp"

int main(int argc, char* argv[])
{
    std::cout << "Path: " << argv[0] << std::endl;

    Controller controller(new ModelMain(), new ViewMain());
    return controller.Run();
}
