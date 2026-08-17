#include "pressure_index_detector.hpp"

#include <gtest/gtest.h>

#include <vector>

constexpr float kFirstSampleBallY{1.0f};
constexpr float kSecondSampleBallY{3.0f};

class PressureIndexDetectorTest : public ::testing::Test
{
  protected:
    PressureIndexDetector detector_;
};

TEST_F(PressureIndexDetectorTest, StartsEmpty)
{
    EXPECT_TRUE(detector_.GetBallPossession().empty());
}

TEST_F(PressureIndexDetectorTest, RecordSampleAppendsValuesInOrder)
{
    detector_.RecordSample(kFirstSampleBallY);
    detector_.RecordSample(kSecondSampleBallY);

    const std::vector<float> expected_ball_possession{kFirstSampleBallY, kSecondSampleBallY};
    EXPECT_EQ(detector_.GetBallPossession(), expected_ball_possession);
}
