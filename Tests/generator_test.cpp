#include <gtest/gtest.h>

#include <algorithm>
#include <array>
#include <ranges>
#include <string>
#include <vector>

#include "model_main.hpp"
#include "player.hpp"

TEST(ModelMainTest, GenerateTeamsCreatesTwoPairsForFourPlayers)
{
    ModelMain model;

    const generator::Players input_players{
      Player{" Alice "},
      Player{"Bob"},
      Player{"Carol"},
      Player{"Dave"},
    };

    const auto draw = model.GenerateTeams(input_players);

    ASSERT_TRUE(draw.has_value());
    EXPECT_FALSE(draw->first_team.first_player.GetNickname().empty());
    EXPECT_FALSE(draw->first_team.second_player.GetNickname().empty());
    EXPECT_FALSE(draw->second_team.first_player.GetNickname().empty());
    EXPECT_FALSE(draw->second_team.second_player.GetNickname().empty());

    std::vector<std::string> actual_players{
      draw->first_team.first_player.GetNickname(),
      draw->first_team.second_player.GetNickname(),
      draw->second_team.first_player.GetNickname(),
      draw->second_team.second_player.GetNickname(),
    };

    std::vector<std::string> expected_players;
    expected_players.reserve(input_players.size());
    for (const auto& player : input_players)
    {
        expected_players.push_back(player.GetNickname());
    }

    EXPECT_EQ(actual_players.size(), expected_players.size());
    EXPECT_TRUE(std::ranges::is_permutation(actual_players, expected_players));
}
