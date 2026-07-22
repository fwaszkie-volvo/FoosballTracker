#ifndef FOOSBALL_TRACKER_CONTROLLER_HPP_
#define FOOSBALL_TRACKER_CONTROLLER_HPP_

#include <memory>
#include <utility>

#include "model_main.hpp"
#include "view_main.hpp"

class Controller
{
  public:
    Controller(std::unique_ptr<ModelMain> model, std::unique_ptr<ViewMain> view)
        : model_{std::move(model)}, view_{std::move(view)}
    {
    }

    int Run();

  private:
    std::unique_ptr<ModelMain> model_;
    std::unique_ptr<ViewMain> view_;
};

#endif  // FOOSBALL_TRACKER_CONTROLLER_HPP_