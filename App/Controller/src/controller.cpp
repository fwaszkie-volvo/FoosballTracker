#include "controller.hpp"

#include <spdlog/spdlog.h>

#include <functional>
#include <opencv2/core/mat.hpp>
#include <optional>
#include <thread>

int Controller::Run(int argc, char* argv[])
{
    view_->SetOnFileLoaded([this](const std::string& path) { LoadFileToAnalysis(path); });
    view_->SetOnAnalyseClicked([this]() { AnalyseOfflineFile(); });
    view_->SetOnLiveClicked([this]() { StartLive(); });
    view_->SetOnSave([this](const std::string& path) { SaveResult(path); });
    return view_->CreateAndRunMain(argc, argv);
}

void Controller::LoadFileToAnalysis(const std::string& path)
{
    model_->LoadFile(path);
    view_->DrawVideo(path);
}

void Controller::AnalyseOfflineFile()
{
    // anal_thread_ = std::thread(
    //   [this, work = [this]() { model_->CalculateFromFile(); }]()
    //   {
    //       work();
    //       view_->CreateAnalWindowInProgress([this]()
    //                                         { view_->DrawVideo(model_->GetTempOutputPath()); });
    //   });

    // anal_thread_.detach();
}

void Controller::StartLive()
{
    model_->CalculateFromStream();
    // tu trzeba pomyśleć jak stream na żywo przekazać
}

void Controller::SaveResult(const std::string& path) { model_->SaveResult(path); }