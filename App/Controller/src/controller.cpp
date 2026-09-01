#include "controller.hpp"

#include <spdlog/spdlog.h>

#include <array>
#include <cstddef>
#include <functional>
#include <nlohmann/detail/json_ref.hpp>
#include <nlohmann/json.hpp>
#include <nlohmann/json_fwd.hpp>
#include <thread>

#include "generator_types.hpp"
#include "model_types.hpp"
#include "player.hpp"

namespace
{
nlohmann::json PlayerJson(const Player& player)
{
    return {{"nickname", player.GetNickname()}, {"elo", player.GetElo()}};
}

nlohmann::json TeamJson(const model::Team& team)
{
    return {{"players", {PlayerJson(team.players.first), PlayerJson(team.players.second)}}};
}

nlohmann::json PositionJson(const model::Team& team)
{
    return {{"defence", team.players.first.GetNickname()},
            {"offence", team.players.second.GetNickname()}};
}

nlohmann::json SettingsJson(const model::Teams& first_set)
{
    return {{"red", PositionJson(first_set.first)}, {"blue", PositionJson(first_set.second)}};
}
}  // namespace

int Controller::Run()
{
    view_->SetOnFileLoaded([this](const std::string& path) { LoadFileToAnalysis(path); });
    view_->SetOnAnalyseClicked([this]() { AnalyseOfflineFile(); });
    view_->SetOnLiveClicked([this]() { StartLive(); });
    view_->SetOnSave([this](const std::string& path) { SaveResult(path); });
    view_->SetOnCreatePlayer([this](const std::string& nickname)
                             { return CreatePlayer(nickname); });
    view_->SetOnCheckPlayer([this](const std::string& nickname) { return CheckPlayer(nickname); });
    view_->SetOnGenerateTeams(
      [this](
        const std::vector<std::string>& nicknames, const bool by_elo, const std::string& formation)
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

bool Controller::CreatePlayer(const std::string& nickname)
{
    return model_->CreatePlayer(nickname);
}

std::optional<int> Controller::CheckPlayer(const std::string& nickname)
{
    const auto player = model_->GetPlayer(nickname);
    return player ? std::optional<int>{player->GetElo()} : std::nullopt;
}

std::pair<int, std::string> Controller::GenerateTeams(const std::vector<std::string>& nicknames,
                                                      const bool by_elo,
                                                      const std::string& formation)
{
    std::array<std::optional<Player>, generator::kPlayersCount> registered_players;
    for (std::size_t index = 0U; index < registered_players.size(); ++index)
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

    std::optional<nlohmann::json> settings_json;
    if (formation == "standard")
    {
        const auto team_settings = model_->GenerateTeamSettingsStandard(*teams);
        if (team_settings)
        {
            settings_json = SettingsJson(team_settings->at(0));
        }
    }
    else if (formation == "random")
    {
        const auto team_settings = model_->GenerateTeamSettingsRandom(*teams);
        if (team_settings)
        {
            settings_json = SettingsJson(team_settings->at(0));
        }
    }

    nlohmann::json response = {{"teams", {TeamJson(teams->first), TeamJson(teams->second)}}};
    if (settings_json)
    {
        response["settings"] = *settings_json;
    }

    return {200, response.dump()};
}