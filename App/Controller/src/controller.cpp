#include "controller.hpp"

#include <functional>
#include <opencv2/core/mat.hpp>
#include <optional>

void Controller::SetUp()
{
    view_->SetOnFileLoaded([this](const std::string& path) { LoadFileToAnalysis(path); });
    view_->SetOnAnalyseClicked([this]() { AnalyseOfflineFile(); });
    view_->SetOnLiveClicked([this]() { StartLive(); });
    view_->SetOnSave([this](const std::string& path) { SaveResult(path); });
    view_->Draw(std::nullopt);
}

void Controller::LoadFileToAnalysis(const std::string& path)
{
    model_->LoadFile(path);
    view_->DrawVideo(path);
}

void Controller::AnalyseOfflineFile()
{
    view_->RunWithProgress(
      "ANAL IN PROGRESS",
      [this]() { model_->CalculateFromFile(); },
      [this]() { view_->DrawVideo(model_->GetTempOutputPath()); });
}

void Controller::StartLive()
{
    model_->CalculateFromStream();
    // tu trzeba pwymyśleć jak stream na żywo przekazać
}

void Controller::SaveResult(const std::string& path) { model_->SaveResult(path); }