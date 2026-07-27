#include "photo-reader.hpp"

bool PhotoReader::Open(const std::string& source)
{
    image_ = cv::imread(source, cv::IMREAD_COLOR);
    consumed_ = false;
    return !image_.empty();
}

std::optional<cv::Mat> PhotoReader::Read()
{
    if (image_.empty() || consumed_)
    {
        return std::nullopt;
    }

    consumed_ = true;
    return image_.clone();
}

bool PhotoReader::IsOpened() const { return !image_.empty(); }

std::optional<double> PhotoReader::GetFps() const { return fps_; }

std::optional<cv::Size> PhotoReader::GetFrameSize() const { return image_.size(); }
