#include "generator.hpp"

#include <gtest/gtest.h>

#include <algorithm>
#include <string>
#include <vector>

TEST(ModelMainTest, GenerateTeamsCreatesTwoPairsForFourPlayers)
{
    const std::array<Player, generator::kPlayersCount> input_players{
      Player{"Alice"},
      Player{"Bob"},
      Player{"Carol"},
      Player{"Dave"},
    };

    const auto draw = generator::GenerateTeams(input_players);

    ASSERT_TRUE(draw.has_value());
    EXPECT_FALSE(draw->first.players.first.GetNickname().empty());
    EXPECT_FALSE(draw->first.players.second.GetNickname().empty());
    EXPECT_FALSE(draw->second.players.first.GetNickname().empty());
    EXPECT_FALSE(draw->second.players.second.GetNickname().empty());

    std::vector<std::string> actual_players{
      draw->first.players.first.GetNickname(),
      draw->first.players.second.GetNickname(),
      draw->second.players.first.GetNickname(),
      draw->second.players.second.GetNickname(),
    };

    std::vector<std::string> expected_players;
    expected_players.reserve(input_players.size());
    for (const auto& player : input_players)
    {
        expected_players.push_back(player.GetNickname());
    }

    EXPECT_EQ(actual_players.size(), expected_players.size());
    EXPECT_TRUE(std::is_permutation(actual_players.begin(),
                                    actual_players.end(),
                                    expected_players.begin(),
                                    expected_players.end()));
}
