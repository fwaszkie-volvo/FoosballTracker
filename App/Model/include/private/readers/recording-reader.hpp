#ifndef INPUT_RECORDING_READER_HPP_
#define INPUT_RECORDING_READER_HPP_

#include "frame-reader.hpp"

class RecordingReader : public IFrameReader
{
  public:
    bool Open(const std::string& source) override;
    std::optional<cv::Mat> Read() override;
    bool IsOpened() const override;
    std::optional<double> GetFps() const override;
    std::optional<cv::Size> GetFrameSize() const override;

  private:
    cv::VideoCapture capture_;
};

#endif  // INPUT_RECORDING_READER_HPP_
