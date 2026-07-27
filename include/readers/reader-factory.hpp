#ifndef INPUT_READER_FACTORY_HPP_
#define INPUT_READER_FACTORY_HPP_

#include <cstdint>
#include <memory>

#include "frame-reader.hpp"

enum class ReaderType : std::uint8_t
{
    kRecording,
    kOnline,
    kPhoto,
};

std::unique_ptr<IFrameReader> CreateReader(const ReaderType type);

#endif  /* INPUT_READER_FACTORY_HPP_ */
