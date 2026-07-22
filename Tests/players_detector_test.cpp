#include "players_detector.hpp"

#include <gtest/gtest.h>

#include <filesystem>

TEST(PlayersDetector, CorrectPlayersAmount)
{
    std::string image_path = std::string(TEST_SOURCE_DIR) + "/test_files/ball_unobscured.jpg";
    const auto output_dir = std::filesystem::path(TEST_SOURCE_DIR) / "test_outputs";
    cv::Mat frame = cv::imread(image_path, cv::IMREAD_COLOR);
    if (frame.empty())
    {
        std::cerr << "Error: Could not open or find the image." << std::endl;
    }

    PlayersDetector players_detector{};

    players_detector.DetectPlayers(frame);
    players_detector.DisplayPlayers(frame);
    cv::imwrite(output_dir / "detect_players_output.jpg", frame);

    size_t team_blue_count{players_detector.players_.rectangles_blue_.size()};
    size_t team_red_count{players_detector.players_.rectangles_red_.size()};

    size_t constexpr actual_team_count = 11;
    EXPECT_EQ(team_blue_count, actual_team_count);
    EXPECT_EQ(team_red_count, actual_team_count);
}
