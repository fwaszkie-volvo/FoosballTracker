#include "ball_position_recorder.hpp"

#include <gtest/gtest.h>

#include <vector>

constexpr float kFirstSampleBallY{1.0f};
constexpr float kSecondSampleBallY{3.0f};

class BallPositionRecorderTest : public ::testing::Test
{
  protected:
    BallPositionRecorder recorder_;
};

TEST_F(BallPositionRecorderTest, StartsEmpty)
{
    EXPECT_TRUE(recorder_.GetPositions().empty());
}

TEST_F(BallPositionRecorderTest, RecordSampleAppendsValuesInOrder)
{
    recorder_.RecordSample(kFirstSampleBallY);
    recorder_.RecordSample(kSecondSampleBallY);

    const std::vector<float> expected_positions{kFirstSampleBallY, kSecondSampleBallY};
    EXPECT_EQ(recorder_.GetPositions(), expected_positions);
}
