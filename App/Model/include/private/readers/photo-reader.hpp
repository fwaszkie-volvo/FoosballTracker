#ifndef INPUT_PHOTO_READER_HPP_
#define INPUT_PHOTO_READER_HPP_

#include "frame-reader.hpp"

class PhotoReader : public IFrameReader
{
  public:
    bool Open(const std::string& source) override;
    std::optional<cv::Mat> Read() override;
    bool IsOpened() const override;
    std::optional<double> GetFps() const override;
    std::optional<cv::Size> GetFrameSize() const override;

  private:
    cv::Mat image_;
    bool consumed_{};
    double fps_{1.0};
};

#endif  // INPUT_PHOTO_READER_HPP_
