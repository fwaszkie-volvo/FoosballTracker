#include "frame_processor.hpp"

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

std::optional<cv::Mat> FrameProcessor::ProcessFrames(const config::ProcessingTarget& target,
                                                     const FrameHandler& frame_processor)
{
    reader_ = CreateReader(reader_type_);
    if (!reader_)
    {
        return std::nullopt;
    }

    if (!reader_->Open(target.source))
    {
        return std::nullopt;
    }

    cv::VideoWriter output_writer;

    std::error_code error;
    const auto output_directory = std::filesystem::path(target.output_path).parent_path();
    if (!output_directory.empty())
    {
        std::filesystem::create_directories(output_directory, error);
    }

    auto last_frame = ProcessInputFrames(output_writer, target.output_path, frame_processor);

    if (reader_type_ == ReaderType::kPhoto && HasValidFrame(last_frame))
    {
        cv::imwrite(target.output_path, last_frame.value());
    }
    return last_frame;
}

std::optional<cv::Mat> FrameProcessor::ProcessInputFrames(cv::VideoWriter& output_writer,
                                                          const std::string& output_path,
                                                          const FrameHandler& frame_processor)
{
    std::optional<cv::Mat> last_frame;

    while (true)
    {
        auto frame = reader_->Read();
        if (!HasValidFrame(frame))
        {
            break;
        }

        frame_processor(frame.value());

        if (!TryWriteOutputFrame(output_writer, output_path, frame.value()))
        {
            return std::nullopt;
        }

        last_frame = std::move(frame);
    }

    return last_frame;
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
