#ifndef FOOSBALL_TRACKER_APP_CONTROLLER_INCLUDE_CONTROLLER_HPP_
#define FOOSBALL_TRACKER_APP_CONTROLLER_INCLUDE_CONTROLLER_HPP_

#include <memory>
#include <string>
#include <thread>
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

    ~Controller()
    {
        if (anal_thread_.joinable())
        {
            anal_thread_.join();
        }
    }

    int Run();
    void LoadFileToAnalysis(const std::string& path);
    void AnalyseOfflineFile();
    void StartLive();
    void SaveResult(const std::string& path);
    bool CreatePlayer(const std::string& nickname);
    std::optional<int> CheckPlayer(const std::string& nickname);
    std::pair<int, std::string> GenerateTeams(const std::vector<std::string>& nicknames,
                          bool by_elo, const std::string& formation);

  private:
    std::unique_ptr<ModelMain> model_;
    std::unique_ptr<ViewMain> view_;
    std::thread anal_thread_;
};

#endif  // FOOSBALL_TRACKER_APP_CONTROLLER_INCLUDE_CONTROLLER_HPP_
