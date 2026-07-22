#ifndef FOOSBALL_TRACKER_APP_MODEL_INCLUDE_PRIVATE_READERS_VIDEO_FRAME_READER_HPP_
#define FOOSBALL_TRACKER_APP_MODEL_INCLUDE_PRIVATE_READERS_VIDEO_FRAME_READER_HPP_

#include "frame_reader.hpp"

class VideoFrameReader : public IFrameReader
{
  public:
    std::optional<cv::Mat> Read() override
    {
        if (!capture_.isOpened())
        {
            return std::nullopt;
        }

        cv::Mat frame;
        capture_ >> frame;
        if (frame.empty())
        {
            return std::nullopt;
        }

        return frame;
    }

    bool IsOpened() const override { return capture_.isOpened(); }

    std::optional<double> GetFps() const override
    {
        if (!capture_.isOpened())
        {
            return std::nullopt;
        }

        return capture_.get(cv::CAP_PROP_FPS);
    }

    std::optional<cv::Size> GetFrameSize() const override
    {
        if (!capture_.isOpened())
        {
            return std::nullopt;
        }

        const int width = static_cast<int>(capture_.get(cv::CAP_PROP_FRAME_WIDTH));
        const int height = static_cast<int>(capture_.get(cv::CAP_PROP_FRAME_HEIGHT));
        return cv::Size(width, height);
    }

  protected:
    cv::VideoCapture capture_;
};

#endif  // FOOSBALL_TRACKER_APP_MODEL_INCLUDE_PRIVATE_READERS_VIDEO_FRAME_READER_HPP_
