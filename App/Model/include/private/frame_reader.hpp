#ifndef FOOSBALL_TRACKER_FRAME_READER_HPP_
#define FOOSBALL_TRACKER_FRAME_READER_HPP_

#include <functional>
#include <memory>
#include <opencv2/opencv.hpp>
#include <optional>
#include <string>

#include "reader-factory.hpp"

class FrameProcessor
{
  public:
    using FrameHandler = std::function<void(cv::Mat&)>;

    void SetReaderType(const ReaderType reader_type);
    std::optional<cv::Mat> ProcessFrames(const std::string& source,
                                         const std::string& output_path,
                                         const FrameHandler& frame_processor);

  private:
    static bool HasValidFrame(const std::optional<cv::Mat>& frame);

    std::optional<cv::Mat> ProcessInputFrames(cv::VideoWriter& output_writer,
                                              const std::string& output_path,
                                              const FrameHandler& frame_processor);

    bool TryWriteOutputFrame(cv::VideoWriter& output_writer,
                             const std::string& output_path,
                             const cv::Mat& frame) const;

    ReaderType reader_type_{ReaderType::kUnspecified};
    std::unique_ptr<IFrameReader> reader_;
};

#endif  // FOOSBALL_TRACKER_FRAME_READER_HPP_
