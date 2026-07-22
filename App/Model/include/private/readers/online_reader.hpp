#ifndef FOOSBALL_TRACKER_APP_MODEL_INCLUDE_PRIVATE_READERS_ONLINE_READER_HPP_
#define FOOSBALL_TRACKER_APP_MODEL_INCLUDE_PRIVATE_READERS_ONLINE_READER_HPP_

#include "video_frame_reader.hpp"

class OnlineReader : public VideoFrameReader
{
  public:
    bool Open(const std::string& source) override;
};

#endif  // FOOSBALL_TRACKER_APP_MODEL_INCLUDE_PRIVATE_READERS_ONLINE_READER_HPP_
