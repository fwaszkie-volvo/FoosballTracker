#include "pressure_index_detector.hpp"

#include <gtest/gtest.h>

#include <vector>

constexpr float kFirstSampleRed{1.0f};
constexpr float kFirstSampleBlue{2.0f};
constexpr float kSecondSampleRed{3.0f};
constexpr float kSecondSampleBlue{4.0f};

class PressureIndexDetectorTest : public ::testing::Test
{
  protected:
    PressureIndexDetector detector_;
};

TEST_F(PressureIndexDetectorTest, StartsEmpty)
{
    EXPECT_TRUE(detector_.GetRedPressure().empty());
    EXPECT_TRUE(detector_.GetBluePressure().empty());
}

TEST_F(PressureIndexDetectorTest, RecordSampleAppendsValuesInOrder)
{
    detector_.RecordSample(kFirstSampleRed, kFirstSampleBlue);
    detector_.RecordSample(kSecondSampleRed, kSecondSampleBlue);

    const std::vector<float> expected_red{kFirstSampleRed, kSecondSampleRed};
    const std::vector<float> expected_blue{kFirstSampleBlue, kSecondSampleBlue};
    EXPECT_EQ(detector_.GetRedPressure(), expected_red);
    EXPECT_EQ(detector_.GetBluePressure(), expected_blue);
}
