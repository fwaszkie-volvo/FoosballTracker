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
  const generator::Players& players) const
{
    return generator::GenerateTeams(players);
}
