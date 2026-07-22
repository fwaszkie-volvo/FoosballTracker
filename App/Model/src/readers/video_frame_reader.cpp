#include "video_frame_reader.hpp"

std::optional<cv::Mat> VideoFrameReader::Read()
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

bool VideoFrameReader::IsOpened() const { return capture_.isOpened(); }

std::optional<double> VideoFrameReader::GetFps() const
{
    if (!capture_.isOpened())
    {
        return std::nullopt;
    }

    return capture_.get(cv::CAP_PROP_FPS);
}

std::optional<cv::Size> VideoFrameReader::GetFrameSize() const
{
    if (!capture_.isOpened())
    {
        return std::nullopt;
    }

    const int width = static_cast<int>(capture_.get(cv::CAP_PROP_FRAME_WIDTH));
    const int height = static_cast<int>(capture_.get(cv::CAP_PROP_FRAME_HEIGHT));
    return cv::Size(width, height);
}
