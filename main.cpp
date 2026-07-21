#include "controller.hpp"
#include "model_main.hpp"
#include "view_main.hpp"

int main()
{
    Controller controller(new ModelMain(), new ViewMain());
    return controller.Run();
}
