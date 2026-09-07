#include "controller.hpp"

#include <spdlog/spdlog.h>

#include <array>
#include <cstddef>
#include <functional>
#include <nlohmann/detail/json_ref.hpp>
#include <nlohmann/json.hpp>
#include <nlohmann/json_fwd.hpp>
#include <thread>

#include "controller_json.hpp"
#include "generator_types.hpp"
#include "model_types.hpp"
#include "player.hpp"

int Controller::Run()
{
    view_->SetOnFileLoaded([this](const std::string& path) { LoadFileToAnalysis(path); });
    view_->SetOnAnalyseClicked([this]() { AnalyseOfflineFile(); });
    view_->SetOnLiveClicked([this]() { StartLive(); });
    view_->SetOnSave([this](const std::string& path) { SaveResult(path); });
    view_->SetOnCreatePlayer(
      [this](const common::Nickname& nickname)
      {
          if (model_->GetPlayer(nickname))
          {
              return false;
          }
          CreatePlayer(nickname);
          return true;
      });
    view_->SetOnCheckPlayer([this](const common::Nickname& nickname)
                            { return CheckPlayer(nickname); });
    view_->SetOnGenerateTeams([this](const std::vector<common::Nickname>& nicknames,
                                     const bool by_elo,
                                     const std::string& formation)
                              { return GenerateTeams(nicknames, by_elo, formation); });
    return view_->CreateAndRunMain();
}

void Controller::LoadFileToAnalysis(const std::string& path)
{
    model_->LoadFile(path);
    view_->DrawVideo(path);
}

void Controller::AnalyseOfflineFile()
{
    if (!model_->CanAnalyzeOfflineFile())
    {
        view_->ShowModalCannotAnalyzeOfflineFile();
        return;
    }

    spdlog::info("AnalyseOfflineFile: start");
    anal_thread_ = std::thread(
      [this, work = [this]() { model_->CalculateFromFile(); }]()
      {
          work();
          view_->HideAnalProgressDialog();
          view_->DrawVideo(model_->GetTempOutputPath());
      });

    anal_thread_.detach();
    view_->ShowAnalProgressDialog();

    spdlog::info("AnalyseOfflineFile: stop");
}

void Controller::StartLive()
{
    model_->CalculateFromStream();
    // tu trzeba pomyśleć jak stream na żywo przekazać
}

void Controller::SaveResult(const std::string& path) { model_->SaveResult(path); }

void Controller::CreatePlayer(const common::Nickname& nickname) { model_->CreatePlayer(nickname); }

std::optional<int> Controller::CheckPlayer(const common::Nickname& nickname)
{
    const auto player = model_->GetPlayer(nickname);
    return player ? std::optional<int>{player->GetElo()} : std::nullopt;
}

HttpResult Controller::GenerateTeams(const std::vector<common::Nickname>& nicknames,
                                     const bool by_elo,
                                     const std::string& formation)
{
    std::array<std::optional<Player>, generator::kPlayersCount> registered_players;
    for (std::size_t index{0U}; index < registered_players.size(); ++index)
    {
        const auto player = model_->GetPlayer(nicknames.at(index));
        if (!player)
        {
            return {
              404,
              nlohmann::json{{"error", "Player '" + nicknames.at(index) + "' does not exist."}}
                .dump()};
        }
        registered_players.at(index).emplace(*player);
    }

    generator::Players players{
      *registered_players.at(0U),
      *registered_players.at(1U),
      *registered_players.at(2U),
      *registered_players.at(3U),
    };
    const auto teams =
      by_elo ? model_->GenerateTeamsByElo(players) : model_->GenerateTeamsRandom(players);
    if (!teams)
    {
        return {400, nlohmann::json{{"error", "Unable to generate teams."}}.dump()};
    }

    std::optional<model::TeamFormations> team_formations;
    if (formation == "standard")
    {
        team_formations = model_->GenerateTeamFormationsStandard(*teams);
    }
    else if (formation == "random")
    {
        team_formations = model_->GenerateTeamFormationsRandom(*teams);
    }

    nlohmann::json response = {
      {"teams",
       {controller_json::TeamJson(teams->first), controller_json::TeamJson(teams->second)}}};
    if (team_formations)
    {
        auto formations_json = nlohmann::json::array();
        for (const auto& set_formation : *team_formations)
        {
            formations_json.push_back(controller_json::FormationJson(*teams, set_formation));
        }
        response["formation"] = formations_json;
    }

    return {200, response.dump()};
}