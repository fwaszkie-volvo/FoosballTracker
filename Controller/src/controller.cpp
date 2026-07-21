#include "controller.hpp"

int Controller::Run()
{
    model_->Calculate();
    view_->Draw();
    return 0;
}