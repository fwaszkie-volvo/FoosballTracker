#ifndef FOOSBALL_TRACKER_CONTROLLER_HPP_
#define FOOSBALL_TRACKER_CONTROLLER_HPP_

#include <memory>

#include "model_main.hpp"
#include "view_main.hpp"

class Controller
{
  public:
    Controller(ModelMain *model, ViewMain *view) : model_{model}, view_{view} {}
    int Run();

  private:
    std::unique_ptr<ModelMain> model_;
    std::unique_ptr<ViewMain> view_;
};

#endif  // FOOSBALL_TRACKER_CONTROLLER_HPP_