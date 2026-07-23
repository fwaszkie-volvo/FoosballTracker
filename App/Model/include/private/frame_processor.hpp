#ifndef FOOSBALL_TRACKER_APP_MODEL_INCLUDE_PRIVATE_FRAME_PROCESSOR_HPP_
#define FOOSBALL_TRACKER_APP_MODEL_INCLUDE_PRIVATE_FRAME_PROCESSOR_HPP_

#include <functional>
#include <memory>
#include <opencv2/core/mat.hpp>
#include <opencv2/core/types.hpp>
#include <opencv2/videoio.hpp>
#include <optional>
#include <string>

#include "frame_reader.hpp"
#include "processing_config.hpp"
#include "reader_factory.hpp"

class FrameProcessor
{
  public:
    using FrameHandler = std::function<void(cv::Mat&)>;

    void SetReaderType(const ReaderType reader_type);
    std::optional<cv::Mat> ProcessFrames(const config::ProcessingTarget& target,
                                         const FrameHandler& frame_processor);

  private:
    static bool HasValidFrame(const std::optional<cv::Mat>& frame);

    std::optional<cv::Mat> ProcessInputFrames(cv::VideoWriter& output_writer,
                                              const std::string& output_path,
                                              const FrameHandler& frame_processor);

    static bool TryOpenOutputWriter(cv::VideoWriter& output_writer,
                                    const std::string& output_path,
                                    double fps,
                                    const cv::Size& frame_size);

    bool TryWriteOutputFrame(cv::VideoWriter& output_writer,
                             const std::string& output_path,
                             const cv::Mat& frame) const;

    ReaderType reader_type_{ReaderType::kUnspecified};
    std::unique_ptr<IFrameReader> reader_;
};

#endif  // FOOSBALL_TRACKER_APP_MODEL_INCLUDE_PRIVATE_FRAME_PROCESSOR_HPP_
