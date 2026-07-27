#ifndef INPUT_FRAME_READER_HPP_
#define INPUT_FRAME_READER_HPP_

#include <opencv2/opencv.hpp>
#include <optional>
#include <string>

class IFrameReader
{
  public:
    virtual ~IFrameReader() = default;

    virtual bool Open(const std::string& source) = 0;
    virtual std::optional<cv::Mat> Read() = 0;
    virtual bool IsOpened() const = 0;
    virtual std::optional<double> GetFps() const = 0;
    virtual std::optional<cv::Size> GetFrameSize() const = 0;
};

#endif  /* INPUT_FRAME_READER_HPP_ */
