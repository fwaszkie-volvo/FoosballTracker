#include "model_main.hpp"

#include <spdlog/spdlog.h>

#include <opencv2/core/mat.hpp>
#include <optional>
#include <string>

#include "detector.hpp"
#include "frame_processor.hpp"
#include "generator.hpp"
#include "processing_config.hpp"

std::optional<cv::Mat> ModelMain::Calculate()
{
    const auto config = config::kProcessingConfigOnline;

    FrameProcessor frame_processor;
    frame_processor.SetReaderType(config.reader_type);
    const config::ProcessingTarget target{
      .input_source = config.target.input_source,
      .output_path = config.target.output_path,
    };

    const auto frame = frame_processor.ProcessFrames(
      target, [&](cv::Mat& current_frame) { detect_ball(current_frame); });
    if (!frame.has_value() || frame->empty())
    {
        spdlog::error("Failed to process input source: {}", config.target.input_source);
        return std::nullopt;
    }

    spdlog::info("Saved output to: {}", config.target.output_path);
    return frame->clone();
}

std::optional<std::pair<model::Team, model::Team>> ModelMain::GenerateTeamsRandom(
  const generator::Players& players) const
{
    return generator::GenerateTeamsRandom(players);
}

std::optional<std::pair<model::Team, model::Team>> ModelMain::GenerateTeamsByElo(
  const generator::Players& players) const
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
