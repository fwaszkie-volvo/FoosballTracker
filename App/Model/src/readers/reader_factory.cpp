#include "reader_factory.hpp"

#include "online_reader.hpp"
#include "photo_reader.hpp"
#include "recording_reader.hpp"

std::unique_ptr<IFrameReader> CreateReader(const ReaderType type)
{
    switch (type)
    {
        case ReaderType::kUnspecified:
            return nullptr;
        case ReaderType::kRecording:
            return std::make_unique<RecordingReader>();
        case ReaderType::kOnline:
            return std::make_unique<OnlineReader>();
        case ReaderType::kPhoto:
            return std::make_unique<PhotoReader>();
    }

    return nullptr;
}
