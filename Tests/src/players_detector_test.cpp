#include "players_detector.hpp"

#include <gtest/gtest.h>

#include <filesystem>
#include <iostream>
#include <opencv2/opencv.hpp>

TEST(PlayersDetector, CorrectPlayersAmount)
{
    std::string image_path = std::string(TEST_SOURCE_DIR) + "/test_files/ball_unobscured.jpg";
    const auto output_dir  = std::filesystem::path(TEST_SOURCE_DIR) / "test_outputs";
    cv::Mat frame          = cv::imread(image_path, cv::IMREAD_COLOR);
    if (frame.empty())
    {
        std::cerr << "Error: Could not open or find the image." << std::endl;
    }

    PlayersDetector players_detector{};

    players_detector.Detect(frame);
    players_detector.Draw(frame);
    cv::imwrite(output_dir / "detect_players_output.jpg", frame);

    const model::PlayersPositions& players{players_detector.GetPlayers()};

    size_t team_red_count{players.red.offense.size() + players.red.defense.size()};
    size_t team_blue_count{players.blue.offense.size() + players.blue.defense.size()};

    size_t team_blue_defense_count{players.blue.defense.size()};
    size_t team_blue_offense_count{players.blue.offense.size()};
    size_t team_red_defense_count{players.red.defense.size()};
    size_t team_red_offense_count{players.red.offense.size()};

    constexpr size_t actual_team_count = 11;
    EXPECT_EQ(team_blue_count, actual_team_count);
    EXPECT_EQ(team_red_count, actual_team_count);

    constexpr size_t defense_count = 3;
    constexpr size_t offense_count = 8;
    EXPECT_EQ(team_blue_defense_count, defense_count);
    EXPECT_EQ(team_blue_offense_count, offense_count);
    EXPECT_EQ(team_red_defense_count, defense_count);
    EXPECT_EQ(team_red_offense_count, offense_count);
}
