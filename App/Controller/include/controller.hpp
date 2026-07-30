#ifndef FOOSBALL_TRACKER_APP_CONTROLLER_INCLUDE_CONTROLLER_HPP_
#define FOOSBALL_TRACKER_APP_CONTROLLER_INCLUDE_CONTROLLER_HPP_

#include <memory>
#include <string>
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

    void SetUp();
    void LoadFileToAnalysis(const std::string& path);
    void AnalyseOfflineFile();
    void StartLive();
    void SaveResult(const std::string& path);

  private:
    std::unique_ptr<ModelMain> model_;
    std::unique_ptr<ViewMain> view_;
};

#endif  // FOOSBALL_TRACKER_APP_CONTROLLER_INCLUDE_CONTROLLER_HPP_
