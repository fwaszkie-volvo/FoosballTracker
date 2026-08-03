#include "model_main.hpp"

#include <spdlog/spdlog.h>

#include <filesystem>
#include <opencv2/core/mat.hpp>
#include <optional>
#include <system_error>

#include "detector.hpp"
#include "frame_processor.hpp"
#include "generator.hpp"
#include "processing_config.hpp"

void ModelMain::CalculateFromStream()
{
    const auto config = config::kProcessingConfigOnline;

    FrameProcessor frame_processor;
    frame_processor.SetReaderType(config.reader_type);
    const config::ProcessingTarget target{
      .input_source = config.target.input_source,
      .output_path = {},
    };

    frame_processor.ProcessFrames(target,
                                  [&](cv::Mat& current_frame) { detect_ball(current_frame); });

    temp_output_path_ = frame_processor.GetTempOutputPath();
    spdlog::info("CalculateFromStream: zapisano tymczasowo do: {}", temp_output_path_);
}

void ModelMain::LoadFile(const std::string& path)
{
    can_analyze_offline_file_ = true;
    loaded_file_path_ = path;
    spdlog::info("Model: zaladowano plik: {}", loaded_file_path_);
}

void ModelMain::CalculateFromFile()
{
    spdlog::info("Calculate from file: {}", loaded_file_path_);
    if (loaded_file_path_.empty())
    {
        spdlog::error("CalculateFromFile: brak zaladowanego pliku");
        return;
    }

    FrameProcessor frame_processor;
    frame_processor.SetReaderType(ReaderType::kRecording);
    const config::ProcessingTarget target{
      .input_source = loaded_file_path_,
      .output_path = config::kProcessingConfigRecording.target.output_path,
    };

    frame_processor.ProcessFrames(target,
                                  [&](cv::Mat& current_frame) { detect_ball(current_frame); });

    temp_output_path_ = frame_processor.GetTempOutputPath();
    can_analyze_offline_file_ = false;

    spdlog::info("CalculateFromFile: zapisano tymczasowo do: {}", temp_output_path_);
}

bool ModelMain::SaveResult(const std::string& destination) const
{
    if (temp_output_path_.empty())
    {
        spdlog::error("SaveResult: brak tymczasowego pliku do zapisania");
        return false;
    }
    std::error_code ec;
    const auto dest_dir = std::filesystem::path(destination).parent_path();
    if (!dest_dir.empty())
    {
        std::filesystem::create_directories(dest_dir, ec);
    }
    std::filesystem::copy_file(
      temp_output_path_, destination, std::filesystem::copy_options::overwrite_existing, ec);
    if (ec)
    {
        spdlog::error(
          "SaveResult: blad kopiowania {} -> {}: {}", temp_output_path_, destination, ec.message());
        return false;
    }
    spdlog::info("SaveResult: zapisano wynik do: {}", destination);
    return true;
}

std::optional<model::Teams> ModelMain::GenerateTeamsRandom(const generator::Players& players) const
{
    return generator::GenerateTeamsRandom(players);
}

std::optional<model::Teams> ModelMain::GenerateTeamsByElo(const generator::Players& players) const
{
    return generator::GenerateTeamsByElo(players);
}

bool ModelMain::CreatePlayer(const model::Nickname& nickname)
{
    return storage_.CreatePlayer(nickname);
}

bool ModelMain::RecordMatch(const ratings::MatchInput& match)
{
    return storage_.RecordMatch(match);
}

std::optional<Player> ModelMain::GetPlayer(const model::Nickname& nickname) const
{
    return storage_.GetPlayer(nickname);
}

const std::vector<ratings::MatchInput>& ModelMain::GetMatchHistory() const
{
    return storage_.GetMatchHistory();
}
