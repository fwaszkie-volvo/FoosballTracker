#ifndef FOOSBALL_TRACKER_APP_MODEL_INCLUDE_PRIVATE_UTILS_PROCESSING_CONFIG_HPP_
#define FOOSBALL_TRACKER_APP_MODEL_INCLUDE_PRIVATE_UTILS_PROCESSING_CONFIG_HPP_

#include <string>

#include "detector_types.hpp"
#include "reader_factory.hpp"

namespace config
{
struct ProcessingTarget final
{
    std::string input_source;
    std::string output_path;
};

struct ProcessingConfig final
{
    ReaderType reader_type{ReaderType::kUnspecified};
    ProcessingTarget target{};
};

const ProcessingConfig kProcessingConfigPhoto{
  .reader_type = ReaderType::kPhoto,
  .target =
    ProcessingTarget{
      .input_source{detector_types::kInputImagePath},
      .output_path{detector_types::kOutputImagePath},
    },
};

const ProcessingConfig kProcessingConfigRecording{
  .reader_type = ReaderType::kRecording,
  .target =
    ProcessingTarget{
      .input_source{"Tests/test_files/test_video.mp4"},
      .output_path{"Tests/test_outputs/output_recording.mp4"},
    },
};

const ProcessingConfig kProcessingConfigOnline{
  .reader_type = ReaderType::kOnline,
  .target =
    ProcessingTarget{
      .input_source{"rtsp://127.0.0.1:8554/stream"},
      .output_path{"Tests/test_outputs/output_online.mp4"},
    },
};
}  // namespace config

#endif  // FOOSBALL_TRACKER_APP_MODEL_INCLUDE_PRIVATE_UTILS_PROCESSING_CONFIG_HPP_
