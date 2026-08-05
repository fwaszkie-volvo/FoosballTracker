#include "generator.hpp"

#include <gtest/gtest.h>

#include <algorithm>
#include <array>
#include <set>
#include <string>
#include <vector>

namespace
{
void ExpectSetPlayers(const model::Teams& set,
                      const std::array<std::string, generator::kPlayersCount>& expected)
{
    const std::array<std::string, generator::kPlayersCount> actual{
      set.first.players.first.GetNickname(),
      set.first.players.second.GetNickname(),
      set.second.players.first.GetNickname(),
      set.second.players.second.GetNickname(),
    };

    EXPECT_EQ(actual, expected);
}
}  // namespace

TEST(ModelMainTest, GenerateTeamsByEloCreatesTwoPairsForFourPlayers)
{
    const std::array<Player, generator::kPlayersCount> input_players{
      Player{"Alice"},
      Player{"Bob"},
      Player{"Carol"},
      Player{"Dave"},
    };

    const auto draw = generator::GenerateTeamsByElo(input_players);

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

TEST(ModelMainTest, GenerateTeamsRandomCreatesTwoPairsForFourPlayers)
{
    const std::array<Player, generator::kPlayersCount> input_players{
      Player{"Alice"},
      Player{"Bob"},
      Player{"Carol"},
      Player{"Dave"},
    };

    const auto draw = generator::GenerateTeamsRandom(input_players);

    ASSERT_TRUE(draw.has_value());

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

    EXPECT_TRUE(std::is_permutation(actual_players.begin(),
                                    actual_players.end(),
                                    expected_players.begin(),
                                    expected_players.end()));
}

TEST(ModelMainTest, GenerateTeamsByEloBalancesByElo)
{
    const std::array<Player, generator::kPlayersCount> input_players{
      Player{"Top", 1600},
      Player{"MidHigh", 1400},
      Player{"MidLow", 1200},
      Player{"Low", 1000},
    };

    const auto draw = generator::GenerateTeamsByElo(input_players);

    ASSERT_TRUE(draw.has_value());

    const auto first_team_average = draw->first.GetAverageElo();
    const auto second_team_average = draw->second.GetAverageElo();
    EXPECT_DOUBLE_EQ(first_team_average, second_team_average);

    const bool first_team_is_top_low = (draw->first.players.first.GetNickname() == "Top" &&
                                        draw->first.players.second.GetNickname() == "Low") ||
                                       (draw->first.players.first.GetNickname() == "Low" &&
                                        draw->first.players.second.GetNickname() == "Top");

    const bool second_team_is_top_low = (draw->second.players.first.GetNickname() == "Top" &&
                                         draw->second.players.second.GetNickname() == "Low") ||
                                        (draw->second.players.first.GetNickname() == "Low" &&
                                         draw->second.players.second.GetNickname() == "Top");

    EXPECT_TRUE(first_team_is_top_low || second_team_is_top_low);
}

TEST(ModelMainTest, GenerateTeamSettingsRandomCreatesFourNonRepeatingSettings)
{
    const model::Teams input_teams{
      model::Team{{Player{"Alice"}, Player{"Bob"}}},
      model::Team{{Player{"Carol"}, Player{"Dave"}}},
    };

    const auto settings = generator::GenerateTeamSettingsRandom(input_teams);

    ASSERT_TRUE(settings.has_value());
    ASSERT_EQ(settings->size(), model::kSetsPerMatch);

    const std::array<std::string, generator::kPlayersCount> expected_players{
      "Alice", "Bob", "Carol", "Dave"};

    std::set<std::string> signatures;
    for (const auto& teams : *settings)
    {
        const std::array<std::string, generator::kPlayersCount> players{
          teams.first.players.first.GetNickname(),
          teams.first.players.second.GetNickname(),
          teams.second.players.first.GetNickname(),
          teams.second.players.second.GetNickname(),
        };

        EXPECT_TRUE(std::is_permutation(
          players.begin(), players.end(), expected_players.begin(), expected_players.end()));

        signatures.insert(players.at(0) + "|" + players.at(1) + "|" + players.at(2) + "|" +
                          players.at(3));
    }

    EXPECT_EQ(signatures.size(), model::kSetsPerMatch);
}

TEST(ModelMainTest, GenerateTeamSettingsStandardFollowsSwitchPattern)
{
    const model::Teams input_teams{
      model::Team{{Player{"Alice"}, Player{"Bob"}}},
      model::Team{{Player{"Carol"}, Player{"Dave"}}},
    };

    const auto settings = generator::GenerateTeamSettingsStandard(input_teams);

    ASSERT_TRUE(settings.has_value());
    ASSERT_EQ(settings->size(), model::kSetsPerMatch);

    const auto& first_set = settings->at(0);
    const auto& second_set = settings->at(1);
    const auto& third_set = settings->at(2);
    const auto& fourth_set = settings->at(3);

    ExpectSetPlayers(first_set, {"Alice", "Bob", "Carol", "Dave"});
    ExpectSetPlayers(second_set, {"Alice", "Carol", "Bob", "Dave"});
    ExpectSetPlayers(third_set, {"Carol", "Alice", "Bob", "Dave"});
    ExpectSetPlayers(fourth_set, {"Carol", "Alice", "Dave", "Bob"});
}
