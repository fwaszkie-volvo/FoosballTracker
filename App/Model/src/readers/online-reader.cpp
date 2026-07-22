#include "online-reader.hpp"

bool OnlineReader::Open(const std::string& source)
{
    capture_.release();
    if (capture_.open(source, cv::CAP_FFMPEG))
    {
        return true;
    }

    capture_.release();
    if (capture_.open(source, cv::CAP_GSTREAMER))
    {
        return true;
    }
    return false;
}

std::optional<cv::Mat> OnlineReader::Read()
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

bool OnlineReader::IsOpened() const { return capture_.isOpened(); }

std::optional<double> OnlineReader::GetFps() const
{
    if (!capture_.isOpened())
    {
        return std::nullopt;
    }

    return capture_.get(cv::CAP_PROP_FPS);
}

std::optional<cv::Size> OnlineReader::GetFrameSize() const
{
    if (!capture_.isOpened())
    {
        return std::nullopt;
    }

    const int width = static_cast<int>(capture_.get(cv::CAP_PROP_FRAME_WIDTH));
    const int height = static_cast<int>(capture_.get(cv::CAP_PROP_FRAME_HEIGHT));

    return cv::Size(width, height);
}
