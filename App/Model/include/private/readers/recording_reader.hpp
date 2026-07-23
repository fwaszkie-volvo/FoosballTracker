#ifndef FOOSBALL_TRACKER_APP_MODEL_INCLUDE_PRIVATE_READERS_RECORDING_READER_HPP_
#define FOOSBALL_TRACKER_APP_MODEL_INCLUDE_PRIVATE_READERS_RECORDING_READER_HPP_

#include <string>

#include "video_frame_reader.hpp"

class RecordingReader : public VideoFrameReader
{
  public:
    bool Open(const std::string& source) override;
};

#endif  // FOOSBALL_TRACKER_APP_MODEL_INCLUDE_PRIVATE_READERS_RECORDING_READER_HPP_
