#ifndef FOOSBALL_TRACKER_APP_MODEL_INCLUDE_PRIVATE_READERS_READER_FACTORY_HPP_
#define FOOSBALL_TRACKER_APP_MODEL_INCLUDE_PRIVATE_READERS_READER_FACTORY_HPP_

#include <cstdint>
#include <memory>

#include "frame_reader.hpp"

enum class ReaderType : std::uint8_t
{
    kUnspecified,
    kRecording,
    kOnline,
    kPhoto,
};

constexpr bool IsVideoType(const ReaderType reader_type)
{
    return reader_type != ReaderType::kPhoto;
}

std::unique_ptr<IFrameReader> CreateReader(const ReaderType type);

#endif  // FOOSBALL_TRACKER_APP_MODEL_INCLUDE_PRIVATE_READERS_READER_FACTORY_HPP_
