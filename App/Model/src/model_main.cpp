#include "model_main.hpp"

#include <cstdlib>
#include <iostream>
#include <opencv2/core/mat.hpp>
#include <opencv2/core/mat.inl.hpp>
#include <opencv2/highgui.hpp>
#include <optional>
#include <string>

#include "detector.hpp"
#include "frame_processor.hpp"
#include "generator.hpp"
#include "processing_config.hpp"
#include "reader_factory.hpp"

void ModelMain::Calculate()
{
    const auto config = config::kProcessingConfigOnline;
    const char* preview_value = std::getenv("FOOSBALL_PREVIEW");
    const bool is_interactive = (std::getenv("DISPLAY") != nullptr);
    const bool preview_disabled = (preview_value != nullptr) && std::string(preview_value) == "0";
    const bool show_realtime_preview =
      is_interactive && IsVideoType(config.reader_type) && !preview_disabled;

    FrameProcessor frame_processor;
    frame_processor.SetReaderType(config.reader_type);
    const config::ProcessingTarget target{
      .input_source = config.target.input_source,
      .output_path = config.target.output_path,
    };

    const auto frame =
      frame_processor.ProcessFrames(target,
                                    [&](cv::Mat& current_frame)
                                    {
                                        detect_ball(current_frame);

                                        if (show_realtime_preview)
                                        {
                                            cv::imshow("Foosball Tracking output", current_frame);
                                            cv::waitKey(1);
                                        }
                                    });
    if (!frame.has_value() || frame->empty())
    {
        std::cerr << "Failed to process input source: " << config.target.input_source << "\n";
        return;
    }

    if (!IsVideoType(config.reader_type) && is_interactive)
    {
        cv::imshow("Foosball Tracking output", frame.value());
        cv::waitKey(0);
        cv::destroyAllWindows();
    }

    std::cout << "Saved output to: " << config.target.output_path << "\n";
}

std::optional<generator::TeamDraw> ModelMain::GenerateTeams(
  const generator::Players& nicknames) const
{
    return generator::GenerateTeams(nicknames);
}
