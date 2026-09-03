#include <gtest/gtest.h>

#include <stdexcept>
#include <unordered_map>

#include "converters.hpp"
#include "match_elo_calculator.hpp"
#include "player.hpp"

constexpr int kInitialElo{1000};
constexpr int kEloAfterWin{1018};
constexpr int kEloAfterLoss{982};
constexpr int kEloAfterHighMarginWin{1019};
constexpr int kEloAfterHighMarginLoss{981};
constexpr int kPreparationMatches{8};

model::PlayerMap MakePlayers(std::initializer_list<model::Nickname> names)
{
    model::PlayerMap players;
    for (const auto& n : names)
        players.emplace(n, Player{n, kInitialElo});
    return players;
}

ratings::MatchInput Make2v2Match(const model::Teams& teams,
                                 std::uint8_t goals1,
                                 std::uint8_t goals2)
{
    return ratings::MatchInput{
      .teams_         = teams,
      .set_scores_    = {{{goals1, goals1, goals1, goals1}, {goals2, goals2, goals2, goals2}}},
      .team_settings_ = {model::TeamFormation::kStandard,
                         model::TeamFormation::kStandard,
                         model::TeamFormation::kStandard,
                         model::TeamFormation::kStandard},
    };
}

ratings::MatchInput MakeAliceMatch(std::uint8_t goals1, std::uint8_t goals2)
{
    const model::Teams teams{model::Team{.players = {Player{"Alice"}, Player{"Bob"}}},
                             model::Team{.players = {Player{"Carol"}, Player{"Dave"}}}};
    return Make2v2Match(teams, goals1, goals2);
}

bool ApplyMatch(model::PlayerMap& players, const ratings::MatchInput& match)
{
    const auto result = convert::MatchInputFromPlayers(match, players);
    const int delta   = calculator::ComputeFirstTeamEloDelta(result);
    const auto apply  = [&](const Player& p, int d)
    { players.at(p.GetNickname()).SetElo(p.GetElo() + d); };
    apply(result.teams_.first.players.first, delta);
    apply(result.teams_.first.players.second, delta);
    apply(result.teams_.second.players.first, -delta);
    apply(result.teams_.second.players.second, -delta);
    return true;
}

int AliceEloDeltaVsOpponents(const bool strong_opponents,
                             std::uint8_t alice_goals,
                             std::uint8_t opponents_goals)
{
    auto players = MakePlayers({"Alice", "Bob", "Carol", "Dave", "Eve", "Frank"});
    const model::Teams prep_teams =
      strong_opponents ? model::Teams{model::Team{.players = {Player{"Carol"}, Player{"Dave"}}},
                                      model::Team{.players = {Player{"Eve"}, Player{"Frank"}}}}
                       : model::Teams{model::Team{.players = {Player{"Eve"}, Player{"Frank"}}},
                                      model::Team{.players = {Player{"Carol"}, Player{"Dave"}}}};
    for (int i{0}; i < kPreparationMatches; ++i)
        EXPECT_TRUE(ApplyMatch(players, Make2v2Match(prep_teams, 8, 0)));
    EXPECT_TRUE(ApplyMatch(players, MakeAliceMatch(alice_goals, opponents_goals)));
    return players.at("Alice").GetElo() - kInitialElo;
}

TEST(ConvertersRatingsTest, ThrowsWhenAnyPlayerMissing)
{
    auto players = MakePlayers({"Alice", "Bob", "Carol"});
    EXPECT_THROW((void)convert::MatchInputFromPlayers(MakeAliceMatch(8, 6), players),
                 std::out_of_range);
}

TEST(ConvertersRatingsTest, ReturnsValueWhenAllPlayersRegistered)
{
    auto players = MakePlayers({"Alice", "Bob", "Carol", "Dave"});
    EXPECT_NO_THROW((void)convert::MatchInputFromPlayers(MakeAliceMatch(8, 6), players));
}

TEST(ConvertersRatingsTest, MatchInputFromPlayersUsesCurrentElos)
{
    auto players = MakePlayers({"Alice", "Bob", "Carol", "Dave"});
    players.at("Alice").SetElo(1200);

    const auto result = convert::MatchInputFromPlayers(MakeAliceMatch(8, 6), players);

    EXPECT_EQ(result.teams_.first.players.first.GetElo(), 1200);
    EXPECT_EQ(result.teams_.first.players.second.GetElo(), kInitialElo);
}

TEST(ConvertersRatingsTest, UpdatesEloForWinInTwoVsTwoMatch)
{
    auto players = MakePlayers({"Alice", "Bob", "Carol", "Dave"});
    EXPECT_TRUE(ApplyMatch(players, MakeAliceMatch(8, 6)));

    EXPECT_EQ(players.at("Alice").GetElo(), kEloAfterWin);
    EXPECT_EQ(players.at("Bob").GetElo(), kEloAfterWin);
    EXPECT_EQ(players.at("Carol").GetElo(), kEloAfterLoss);
    EXPECT_EQ(players.at("Dave").GetElo(), kEloAfterLoss);
}

TEST(ConvertersRatingsTest, DoesNotChangeRatingsForDrawWithEqualTeams)
{
    auto players = MakePlayers({"Alice", "Bob", "Carol", "Dave"});
    EXPECT_TRUE(ApplyMatch(players, MakeAliceMatch(5, 5)));

    EXPECT_EQ(players.at("Alice").GetElo(), kInitialElo);
    EXPECT_EQ(players.at("Bob").GetElo(), kInitialElo);
    EXPECT_EQ(players.at("Carol").GetElo(), kInitialElo);
    EXPECT_EQ(players.at("Dave").GetElo(), kInitialElo);
}

TEST(ConvertersRatingsTest, GoalDifferenceIncreasesEloDeltaForHigherMargin)
{
    auto players = MakePlayers({"Alice", "Bob", "Carol", "Dave"});
    EXPECT_TRUE(ApplyMatch(players, MakeAliceMatch(8, 5)));

    EXPECT_EQ(players.at("Alice").GetElo(), kEloAfterHighMarginWin);
    EXPECT_EQ(players.at("Bob").GetElo(), kEloAfterHighMarginWin);
    EXPECT_EQ(players.at("Carol").GetElo(), kEloAfterHighMarginLoss);
    EXPECT_EQ(players.at("Dave").GetElo(), kEloAfterHighMarginLoss);
}

TEST(ConvertersRatingsTest, EloChangeDependsOnOpponentStrength)
{
    EXPECT_GT(AliceEloDeltaVsOpponents(true, 8, 6), AliceEloDeltaVsOpponents(false, 8, 6));
    EXPECT_GT(-AliceEloDeltaVsOpponents(false, 8, 10), -AliceEloDeltaVsOpponents(true, 8, 10));
}
