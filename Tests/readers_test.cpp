#include <gtest/gtest.h>

#include <filesystem>
#include <string>

#include "online_reader.hpp"
#include "photo_reader.hpp"
#include "reader_factory.hpp"
#include "recording_reader.hpp"

namespace
{
std::string TestFile(const std::string& file_name)
{
    return (std::filesystem::path(TEST_SOURCE_DIR) / "test_files" / file_name).string();
}
}  // namespace

TEST(ReaderFactoryTest, CreatesExpectedReaderTypes)
{
    EXPECT_NE(CreateReader(ReaderType::kRecording), nullptr);
    EXPECT_NE(CreateReader(ReaderType::kOnline), nullptr);
    EXPECT_NE(CreateReader(ReaderType::kPhoto), nullptr);
}

TEST(PhotoReaderTest, ReadsExactlyOneFrameFromImage)
{
    PhotoReader reader;
    ASSERT_TRUE(reader.Open(TestFile("ball_unobscured.jpg")));

    auto first = reader.Read();
    ASSERT_TRUE(first.has_value());
    EXPECT_FALSE(first->empty());

    auto second = reader.Read();
    EXPECT_FALSE(second.has_value());
}

TEST(PhotoReaderTest, InvalidImagePathFailsToOpen)
{
    PhotoReader reader;
    EXPECT_FALSE(reader.Open(TestFile("missing.jpg")));
    EXPECT_FALSE(reader.IsOpened());
    EXPECT_FALSE(reader.Read().has_value());
}

TEST(PhotoReaderTest, ReopenResetsConsumedState)
{
    PhotoReader reader;
    ASSERT_TRUE(reader.Open(TestFile("ball_unobscured.jpg")));
    ASSERT_TRUE(reader.Read().has_value());
    EXPECT_FALSE(reader.Read().has_value());

    ASSERT_TRUE(reader.Open(TestFile("ball_unobscured.jpg")));
    EXPECT_TRUE(reader.Read().has_value());
}

TEST(RecordingReaderTest, ReadsFramesFromVideoFile)
{
    RecordingReader reader;
    ASSERT_TRUE(reader.Open(TestFile("test_video.mp4")));

    auto first = reader.Read();
    ASSERT_TRUE(first.has_value());
    EXPECT_FALSE(first->empty());

    auto fps = reader.GetFps();
    ASSERT_TRUE(fps.has_value());
    EXPECT_GT(fps.value(), 0.0);
}

TEST(RecordingReaderTest, ExposesFrameSizeAndIsOpened)
{
    RecordingReader reader;
    ASSERT_TRUE(reader.Open(TestFile("test_video.mp4")));
    EXPECT_TRUE(reader.IsOpened());

    auto size = reader.GetFrameSize();
    ASSERT_TRUE(size.has_value());
    EXPECT_GT(size->width, 0);
    EXPECT_GT(size->height, 0);
}

TEST(RecordingReaderTest, InvalidVideoPathFailsToOpen)
{
    RecordingReader reader;
    EXPECT_FALSE(reader.Open(TestFile("missing.mp4")));
    EXPECT_FALSE(reader.IsOpened());
    EXPECT_FALSE(reader.Read().has_value());
    EXPECT_FALSE(reader.GetFps().has_value());
    EXPECT_FALSE(reader.GetFrameSize().has_value());
}

TEST(RecordingReaderTest, EventuallyReachesEndOfStream)
{
    RecordingReader reader;
    ASSERT_TRUE(reader.Open(TestFile("test_video.mp4")));

    int frame_count = 0;
    while (true)
    {
        auto frame = reader.Read();
        if (!frame.has_value())
        {
            break;
        }
        ++frame_count;
    }

    EXPECT_GT(frame_count, 0);
}

TEST(OnlineReaderTest, InvalidRtspEndpointFailsToOpen)
{
    OnlineReader reader;
    EXPECT_FALSE(reader.Open("rtsp://127.0.0.1:1/nonexistent"));
}

TEST(OnlineReaderTest, InvalidEndpointKeepsReaderClosed)
{
    OnlineReader reader;
    EXPECT_FALSE(reader.Open("rtsp://127.0.0.1:1/nonexistent"));
    EXPECT_FALSE(reader.IsOpened());
    EXPECT_FALSE(reader.Read().has_value());
    EXPECT_FALSE(reader.GetFps().has_value());
    EXPECT_FALSE(reader.GetFrameSize().has_value());
}
