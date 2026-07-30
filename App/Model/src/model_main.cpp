#include "model_main.hpp"

#include <spdlog/spdlog.h>

#include <cstdlib>
#include <iostream>
#include <opencv2/core/mat.hpp>
#include <opencv2/core/mat.inl.hpp>
#include <optional>

#include "detector.hpp"
#include "frame_processor.hpp"
#include "generator.hpp"
#include "processing_config.hpp"
#include "reader_factory.hpp"

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

std::optional<generator::TeamDrawResult> ModelMain::GenerateTeams(
  const std::array<Player, generator::kPlayersCount>& players) const
{
    return generator::GenerateTeams(players);
}

bool ModelMain::CreatePlayer(const model::Nickname& nickname)
{
    return ratings_service_.CreatePlayer(nickname);
}

bool ModelMain::RecordMatch(const ratings::MatchInput& match)
{
    return ratings_service_.RecordMatch(match);
}

std::optional<Player> ModelMain::GetPlayer(const model::Nickname& nickname) const
{
    return ratings_service_.GetPlayer(nickname);
}

const std::vector<ratings::MatchInput>& ModelMain::GetMatchHistory() const
{
    return ratings_service_.GetMatchHistory();
}
