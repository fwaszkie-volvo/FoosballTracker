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
    void ProcessFrames(const std::string& input_stream, const FrameHandler& frame_processor);
    const std::string& GetTempOutputPath() const { return temp_output_path_; }

  private:
    static bool HasValidFrame(const std::optional<cv::Mat>& frame);
    static std::string GenerateTempPath();

    void ProcessInputFrames(const FrameHandler& frame_processor);

    bool TryOpenOutputWriter(cv::VideoWriter& output_writer,
                             double fps,
                             const cv::Size& frame_size) const;

    bool TryWriteOutputFrame(cv::VideoWriter& output_writer, const cv::Mat& frame) const;

    ReaderType reader_type_{ReaderType::kUnspecified};
    std::unique_ptr<IFrameReader> reader_;
    std::string temp_output_path_;
};

#endif  // FOOSBALL_TRACKER_APP_MODEL_INCLUDE_PRIVATE_FRAME_PROCESSOR_HPP_
