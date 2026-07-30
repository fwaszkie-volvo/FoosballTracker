#include "frame_processor.hpp"

#include <unistd.h>

#include <chrono>
#include <filesystem>
#include <opencv2/core/mat.hpp>
#include <opencv2/core/mat.inl.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/videoio.hpp>
#include <system_error>
#include <utility>
#include <vector>

#include "processing_config.hpp"

void FrameProcessor::SetReaderType(const ReaderType reader_type)
{
    reader_type_ = reader_type;
    reader_.reset();
}

std::string FrameProcessor::GenerateTempPath()
{
    const auto now = std::chrono::steady_clock::now().time_since_epoch().count();
    const std::string filename =
      "foosball_" + std::to_string(getpid()) + "_" + std::to_string(now) + ".mp4";
    return (std::filesystem::temp_directory_path() / filename).string();
}

void FrameProcessor::ProcessFrames(const config::ProcessingTarget& target,
                                   const FrameHandler& frame_processor)
{
    reader_ = CreateReader(reader_type_);
    if (!reader_)
    {
        return;
    }

    if (!reader_->Open(target.input_source))
    {
        return;
    }

    // Remove previous temp file if present
    if (!temp_output_path_.empty())
    {
        std::error_code ec;
        std::filesystem::remove(temp_output_path_, ec);
    }
    temp_output_path_ = GenerateTempPath();

    ProcessInputFrames(frame_processor);
}

void FrameProcessor::ProcessInputFrames(const FrameHandler& frame_processor)
{
    cv::VideoWriter output_writer;

    while (true)
    {
        auto frame = reader_->Read();
        if (!HasValidFrame(frame))
        {
            break;
        }

        frame_processor(frame.value());

        if (!TryWriteOutputFrame(output_writer, temp_output_path_, frame.value()))
        {
            return;
        }
    }
}

bool FrameProcessor::HasValidFrame(const std::optional<cv::Mat>& frame)
{
    return frame.has_value() && !frame->empty();
}

bool FrameProcessor::TryWriteOutputFrame(cv::VideoWriter& output_writer,
                                         const std::string& output_path,
                                         const cv::Mat& frame) const
{
    if (reader_type_ == ReaderType::kPhoto)
    {
        return true;
    }

    if (!output_writer.isOpened())
    {
        const double fps = reader_->GetFps().value_or(30.0);
        if (!TryOpenOutputWriter(output_writer, output_path, fps, frame.size()))
        {
            return false;
        }
    }
    output_writer.write(frame);
    return true;
}

bool FrameProcessor::TryOpenOutputWriter(cv::VideoWriter& output_writer,
                                         const std::string& output_path,
                                         const double fps,
                                         const cv::Size& frame_size)
{
    const std::vector<int> codecs = {
      cv::VideoWriter::fourcc('a', 'v', 'c', '1'),
      cv::VideoWriter::fourcc('m', 'p', '4', 'v'),
    };

    for (const int codec : codecs)
    {
        output_writer.release();
        output_writer.open(output_path, codec, fps, frame_size, true);
        if (output_writer.isOpened())
        {
            return true;
        }
    }

    return false;
}
