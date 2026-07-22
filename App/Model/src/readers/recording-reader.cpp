#include "recording-reader.hpp"

bool RecordingReader::Open(const std::string& source) { return capture_.open(source); }

std::optional<cv::Mat> RecordingReader::Read()
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

bool RecordingReader::IsOpened() const { return capture_.isOpened(); }

std::optional<double> RecordingReader::GetFps() const
{
    if (!capture_.isOpened())
    {
        return std::nullopt;
    }

    return capture_.get(cv::CAP_PROP_FPS);
}

std::optional<cv::Size> RecordingReader::GetFrameSize() const
{
    if (!capture_.isOpened())
    {
        return std::nullopt;
    }

    const int width = static_cast<int>(capture_.get(cv::CAP_PROP_FRAME_WIDTH));
    const int height = static_cast<int>(capture_.get(cv::CAP_PROP_FRAME_HEIGHT));
    return cv::Size(width, height);
}
