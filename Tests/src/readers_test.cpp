#include <gtest/gtest.h>

#include <string>

#include "online_reader.hpp"
#include "photo_reader.hpp"
#include "reader_factory.hpp"
#include "recording_reader.hpp"
#include "utils.hpp"

class ReaderTest : public ::testing::Test
{
  protected:
    PhotoReader photo_reader_;
    RecordingReader recording_reader_;
    OnlineReader online_reader_;
};

TEST_F(ReaderTest, CreatesExpectedReaderTypes)
{
    EXPECT_NE(CreateReader(ReaderType::kRecording), nullptr);
    EXPECT_NE(CreateReader(ReaderType::kOnline), nullptr);
    EXPECT_NE(CreateReader(ReaderType::kPhoto), nullptr);
}

TEST_F(ReaderTest, ReadsExactlyOneFrameFromImage)
{
    ASSERT_TRUE(photo_reader_.Open(test_utils::TestFilePath("ball_unobscured.jpg")));

    auto first = photo_reader_.Read();
    ASSERT_TRUE(first.has_value());
    EXPECT_FALSE(first->empty());

    auto second = photo_reader_.Read();
    EXPECT_FALSE(second.has_value());
}

TEST_F(ReaderTest, InvalidImagePathFailsToOpen)
{
    EXPECT_FALSE(photo_reader_.Open(test_utils::TestFilePath("missing.jpg")));
    EXPECT_FALSE(photo_reader_.IsOpened());
    EXPECT_FALSE(photo_reader_.Read().has_value());
}

TEST_F(ReaderTest, ReopenResetsConsumedState)
{
    ASSERT_TRUE(photo_reader_.Open(test_utils::TestFilePath("ball_unobscured.jpg")));
    ASSERT_TRUE(photo_reader_.Read().has_value());
    EXPECT_FALSE(photo_reader_.Read().has_value());

    ASSERT_TRUE(photo_reader_.Open(test_utils::TestFilePath("ball_unobscured.jpg")));
    EXPECT_TRUE(photo_reader_.Read().has_value());
}

TEST_F(ReaderTest, ReadsFramesFromVideoFile)
{
    const auto video_path = test_utils::TestFilePath("test_video_trimmed.mp4");
    ASSERT_TRUE(recording_reader_.Open(video_path));

    auto first = recording_reader_.Read();
    ASSERT_TRUE(first.has_value());
    EXPECT_FALSE(first->empty());

    auto fps = recording_reader_.GetFps();
    ASSERT_TRUE(fps.has_value());
    EXPECT_GT(fps.value(), 0.0);
}

TEST_F(ReaderTest, ExposesFrameSizeAndIsOpened)
{
    const auto video_path = test_utils::TestFilePath("test_video_trimmed.mp4");
    ASSERT_TRUE(recording_reader_.Open(video_path));
    EXPECT_TRUE(recording_reader_.IsOpened());

    auto size = recording_reader_.GetFrameSize();
    ASSERT_TRUE(size.has_value());
    EXPECT_GT(size->width, 0);
    EXPECT_GT(size->height, 0);
}

TEST_F(ReaderTest, InvalidVideoPathFailsToOpen)
{
    EXPECT_FALSE(recording_reader_.Open(test_utils::TestFilePath("missing.mp4")));
    EXPECT_FALSE(recording_reader_.IsOpened());
    EXPECT_FALSE(recording_reader_.Read().has_value());
    EXPECT_FALSE(recording_reader_.GetFps().has_value());
    EXPECT_FALSE(recording_reader_.GetFrameSize().has_value());
}

TEST_F(ReaderTest, EventuallyReachesEndOfStream)
{
    const auto video_path = test_utils::TestFilePath("test_video_trimmed.mp4");
    ASSERT_TRUE(recording_reader_.Open(video_path));

    int frame_count = 0;
    while (true)
    {
        auto frame = recording_reader_.Read();
        if (!frame.has_value())
        {
            break;
        }
        ++frame_count;
    }

    EXPECT_GT(frame_count, 0);
}

TEST_F(ReaderTest, InvalidRtspEndpointFailsToOpen)
{
    EXPECT_FALSE(online_reader_.Open("rtsp://127.0.0.1:1/nonexistent"));
}

TEST_F(ReaderTest, InvalidEndpointKeepsReaderClosed)
{
    EXPECT_FALSE(online_reader_.Open("rtsp://127.0.0.1:1/nonexistent"));
    EXPECT_FALSE(online_reader_.IsOpened());
    EXPECT_FALSE(online_reader_.Read().has_value());
    EXPECT_FALSE(online_reader_.GetFps().has_value());
    EXPECT_FALSE(online_reader_.GetFrameSize().has_value());
}
