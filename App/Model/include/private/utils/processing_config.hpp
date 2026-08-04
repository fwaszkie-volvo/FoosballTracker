#ifndef FOOSBALL_TRACKER_APP_MODEL_INCLUDE_PRIVATE_UTILS_PROCESSING_CONFIG_HPP_
#define FOOSBALL_TRACKER_APP_MODEL_INCLUDE_PRIVATE_UTILS_PROCESSING_CONFIG_HPP_

#include <string>

#include "detector_types.hpp"
#include "reader_factory.hpp"

namespace config
{
struct ProcessingConfig final
{
    ReaderType reader_type{ReaderType::kUnspecified};
    std::string target{};
};

const ProcessingConfig kProcessingConfigOnline{
  .reader_type = ReaderType::kOnline, .target = std::string{"rtsp://127.0.0.1:8554/stream"}};
}  // namespace config

#endif  // FOOSBALL_TRACKER_APP_MODEL_INCLUDE_PRIVATE_UTILS_PROCESSING_CONFIG_HPP_
