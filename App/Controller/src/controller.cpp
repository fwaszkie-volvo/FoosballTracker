#include "controller.hpp"

int Controller::Run()
{
    const auto frame = model_->Calculate();
    view_->Draw(frame);
    return 0;
}