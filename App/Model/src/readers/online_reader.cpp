#include "online_reader.hpp"

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
