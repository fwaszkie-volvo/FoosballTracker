#ifndef FOOSBALL_TRACKER_PROCESSING_CONFIG_HPP_
#define FOOSBALL_TRACKER_PROCESSING_CONFIG_HPP_

#include "detector_config.hpp"
#include "model_main.hpp"

namespace config
{
struct ProcessingConfig
{
    ReaderType reader_type{ReaderType::kPhoto};
    std::string input_source;
    std::string output_path;
};

const ProcessingConfig kProcessingConfigPhoto{
  ReaderType::kPhoto,
  detector_config::kInputImagePath,
  detector_config::kOutputImagePath,
};

const ProcessingConfig kProcessingConfigRecording{
  ReaderType::kRecording,
  "Tests/test_files/test_video.mp4",
  "Tests/test_outputs/output_recording.mp4",
};

const ProcessingConfig kProcessingConfigOnline{
  ReaderType::kOnline,
  "rtsp://127.0.0.1:8554/stream",
  "Tests/test_outputs/output_online.mp4",
};
}  // namespace config

#endif  // FOOSBALL_TRACKER_PROCESSING_CONFIG_HPP_
