#ifndef FOOSBALL_TRACKER_APP_MODEL_INCLUDE_PRIVATE_READERS_VIDEO_FRAME_READER_HPP_
#define FOOSBALL_TRACKER_APP_MODEL_INCLUDE_PRIVATE_READERS_VIDEO_FRAME_READER_HPP_

#include "frame_reader.hpp"

class VideoFrameReader : public IFrameReader
{
  public:
    std::optional<cv::Mat> Read() override;
    bool IsOpened() const override;
    std::optional<double> GetFps() const override;
    std::optional<cv::Size> GetFrameSize() const override;

  protected:
    cv::VideoCapture capture_;
};

#endif  // FOOSBALL_TRACKER_APP_MODEL_INCLUDE_PRIVATE_READERS_VIDEO_FRAME_READER_HPP_
