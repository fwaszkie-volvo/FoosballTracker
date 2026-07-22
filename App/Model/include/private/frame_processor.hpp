#ifndef FOOSBALL_TRACKER_APP_MODEL_INCLUDE_PRIVATE_FRAME_PROCESSOR_HPP_
#define FOOSBALL_TRACKER_APP_MODEL_INCLUDE_PRIVATE_FRAME_PROCESSOR_HPP_

#include <functional>
#include <memory>
#include <opencv2/opencv.hpp>
#include <optional>
#include <string>

#include "reader_factory.hpp"

class FrameProcessor
{
  public:
    struct ProcessingTarget
    {
        std::string source;
        std::string output_path;
    };

    using FrameHandler = std::function<void(cv::Mat&)>;

    void SetReaderType(const ReaderType reader_type);
    std::optional<cv::Mat> ProcessFrames(const ProcessingTarget& target,
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
