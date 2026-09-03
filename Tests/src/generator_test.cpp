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

    const auto first_team_average  = draw->first.GetAverageElo();
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

TEST(ModelMainTest, GenerateTeamFormationsRandomCreatesFourNonRepeatingFormations)
{
    const model::Teams input_teams{
      model::Team{{Player{"Alice"}, Player{"Bob"}}},
      model::Team{{Player{"Carol"}, Player{"Dave"}}},
    };

    const auto formations = generator::GenerateTeamFormationsRandom(input_teams);

    ASSERT_TRUE(formations.has_value());

    for (const auto formation : *formations)
    {
        EXPECT_TRUE(formation == model::TeamFormation::kStandard ||
                    formation == model::TeamFormation::kTeam1Shifted ||
                    formation == model::TeamFormation::kTeam2Shifted ||
                    formation == model::TeamFormation::kBothShifted);
    }

    std::set<model::TeamFormation> unique_formations(formations->begin(), formations->end());
    EXPECT_GE(unique_formations.size(), 1U);
}

TEST(ModelMainTest, GenerateTeamFormationsStandardFollowsSwitchPattern)
{
    const model::Teams input_teams{
      model::Team{{Player{"Alice"}, Player{"Bob"}}},
      model::Team{{Player{"Carol"}, Player{"Dave"}}},
    };

    const auto formations = generator::GenerateTeamFormationsStandard(input_teams);

    ASSERT_TRUE(formations.has_value());

    EXPECT_EQ(formations->at(0), model::TeamFormation::kStandard);
    EXPECT_EQ(formations->at(1), model::TeamFormation::kBothShifted);
    EXPECT_EQ(formations->at(2), model::TeamFormation::kTeam1Shifted);
    EXPECT_EQ(formations->at(3), model::TeamFormation::kTeam2Shifted);
}
