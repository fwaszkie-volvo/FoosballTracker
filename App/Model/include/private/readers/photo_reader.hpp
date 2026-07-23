#ifndef FOOSBALL_TRACKER_APP_MODEL_INCLUDE_PRIVATE_READERS_PHOTO_READER_HPP_
#define FOOSBALL_TRACKER_APP_MODEL_INCLUDE_PRIVATE_READERS_PHOTO_READER_HPP_

#include <opencv2/core/mat.hpp>
#include <opencv2/core/types.hpp>
#include <optional>
#include <string>

#include "frame_reader.hpp"

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

#endif  // FOOSBALL_TRACKER_APP_MODEL_INCLUDE_PRIVATE_READERS_PHOTO_READER_HPP_
