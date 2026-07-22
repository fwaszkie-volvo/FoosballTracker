#include "reader-factory.hpp"

#include "online-reader.hpp"
#include "photo-reader.hpp"
#include "recording-reader.hpp"

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
