#include "online_reader.hpp"

#include <opencv2/videoio.hpp>

bool OnlineReader::Open(const std::string& source)
{
    capture_.release();
    if (capture_.open(source, cv::CAP_FFMPEG))
    {
        return true;
    }

    capture_.release();
    return capture_.open(source, cv::CAP_GSTREAMER);
}
