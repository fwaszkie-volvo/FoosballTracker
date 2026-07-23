#include <gtest/gtest.h>

#include <algorithm>
#include <array>
#include <ranges>
#include <string>
#include <vector>

#include "model_main.hpp"

TEST(ModelMainTest, GenerateTeamsCreatesTwoPairsForFourPlayers)
{
    ModelMain model;

    const auto draw = model.GenerateTeams({" Alice ", "Bob", "Carol", "Dave"});

    ASSERT_TRUE(draw.has_value());
    EXPECT_FALSE(draw->first_team.player_one.empty());
    EXPECT_FALSE(draw->first_team.player_two.empty());
    EXPECT_FALSE(draw->second_team.player_one.empty());
    EXPECT_FALSE(draw->second_team.player_two.empty());

    std::vector<std::string> actual_players{
      draw->first_team.player_one,
      draw->first_team.player_two,
      draw->second_team.player_one,
      draw->second_team.player_two,
    };
    std::ranges::sort(actual_players);

    std::vector<std::string> expected_players{"Alice", "Bob", "Carol", "Dave"};
    std::ranges::sort(expected_players);

    EXPECT_EQ(actual_players, expected_players);
}

TEST(ModelMainTest, GenerateTeamsRejectsInvalidNicknames)
{
    ModelMain model;

    EXPECT_FALSE(model.GenerateTeams({"Alice", "Bob", "Carol"}).has_value());
    EXPECT_FALSE(model.GenerateTeams({"Alice", "Bob", "Alice", "Dave"}).has_value());
    EXPECT_FALSE(model.GenerateTeams({"Alice", "Bob", "   ", "Dave"}).has_value());
}
