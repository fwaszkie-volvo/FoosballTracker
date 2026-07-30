#include "match_storage.hpp"

#include <gtest/gtest.h>

#include <array>
#include <initializer_list>

static constexpr int kInitialElo{1000};
static constexpr int kEloAfterWin{1018};
static constexpr int kEloAfterLoss{982};
static constexpr int kEloAfterHighMarginWin{1019};
static constexpr int kEloAfterHighMarginLoss{981};

static constexpr bool kStrongOpponents{true};
static constexpr bool kWeakOpponents{false};

namespace
{
constexpr int kPreparationMatches{8};

void RegisterPlayers(ratings::MatchStorage& service,
                     const std::initializer_list<model::Nickname>& nicknames)
{
    for (const auto& nickname : nicknames)
    {
        service.CreatePlayer(nickname);
    }
}

ratings::MatchInput BuildMatchResult(const std::uint8_t first_team_goals,
                                     const std::uint8_t second_team_goals)
{
    return ratings::MatchInput{
      .teams_ = {model::Team{.players = {Player{"Alice"}, Player{"Bob"}}},
                 model::Team{.players = {Player{"Carol"}, Player{"Dave"}}}},
      .set_scores_ =
        {{{first_team_goals, first_team_goals, first_team_goals, first_team_goals},
          {second_team_goals, second_team_goals, second_team_goals, second_team_goals}}},
    };
}

void PrepareOpponents(ratings::MatchStorage& service, const bool strong_opponents)
{
    RegisterPlayers(service, {"Carol", "Dave", "Eve", "Frank"});

    for (int index{0}; index < kPreparationMatches; ++index)
    {
        EXPECT_TRUE(service.RecordMatch(ratings::MatchInput{
          .teams_ =
            strong_opponents
              ? std::array<model::Team, model::kTeamsNumber>{model::Team{
                                                               .players = {Player{"Carol"},
                                                                           Player{"Dave"}}},
                                                             model::Team{
                                                               .players = {Player{"Eve"},
                                                                           Player{"Frank"}}}}
              : std::array<model::Team, model::kTeamsNumber>{model::Team{
                                                               .players = {Player{"Eve"},
                                                                           Player{"Frank"}}},
                                                             model::Team{
                                                               .players = {Player{"Carol"},
                                                                           Player{"Dave"}}}},
          .set_scores_ = {{{8, 8, 8, 8}, {0, 0, 0, 0}}},
        }));
    }
}

int AliceEloDeltaAfterMatch(const bool strong_opponents,
                            const std::uint8_t alice_team_goals,
                            const std::uint8_t opponents_goals)
{
    ratings::MatchStorage service;
    RegisterPlayers(service, {"Alice", "Bob", "Carol", "Dave"});
    PrepareOpponents(service, strong_opponents);
    EXPECT_TRUE(service.RecordMatch(BuildMatchResult(alice_team_goals, opponents_goals)));

    const auto alice = service.GetPlayer("Alice");
    EXPECT_TRUE(alice.has_value());
    if (!alice.has_value())
    {
        return 0;
    }

    return alice->GetElo() - kInitialElo;
}
}  // namespace

TEST(MatchStorageTest, CreatesPlayersWithDefaultInitialElo)
{
    ratings::MatchStorage service;
    service.CreatePlayer("Alice");

    const auto alice = service.GetPlayer("Alice");

    ASSERT_TRUE(alice.has_value());
    EXPECT_EQ(alice->GetNickname(), "Alice");
    EXPECT_EQ(alice->GetElo(), kInitialElo);
}

TEST(MatchStorageTest, UpdatesEloForWinInTwoVsTwoMatch)
{
    ratings::MatchStorage service;
    RegisterPlayers(service, {"Alice", "Bob", "Carol", "Dave"});

    EXPECT_TRUE(service.RecordMatch(BuildMatchResult(8, 6)));

    const auto alice = service.GetPlayer("Alice");
    const auto bob = service.GetPlayer("Bob");
    const auto carol = service.GetPlayer("Carol");
    const auto dave = service.GetPlayer("Dave");

    ASSERT_TRUE(alice.has_value());
    ASSERT_TRUE(bob.has_value());
    ASSERT_TRUE(carol.has_value());
    ASSERT_TRUE(dave.has_value());

    EXPECT_EQ(alice->GetElo(), kEloAfterWin);
    EXPECT_EQ(bob->GetElo(), kEloAfterWin);
    EXPECT_EQ(carol->GetElo(), kEloAfterLoss);
    EXPECT_EQ(dave->GetElo(), kEloAfterLoss);
}

TEST(MatchStorageTest, DoesNotChangeRatingsForDrawWithEqualTeams)
{
    ratings::MatchStorage service;
    RegisterPlayers(service, {"Alice", "Bob", "Carol", "Dave"});

    EXPECT_TRUE(service.RecordMatch(BuildMatchResult(5, 5)));

    EXPECT_EQ(service.GetPlayer("Alice")->GetElo(), kInitialElo);
    EXPECT_EQ(service.GetPlayer("Bob")->GetElo(), kInitialElo);
    EXPECT_EQ(service.GetPlayer("Carol")->GetElo(), kInitialElo);
    EXPECT_EQ(service.GetPlayer("Dave")->GetElo(), kInitialElo);
}

TEST(MatchStorageTest, GoalDifferenceIncreasesEloDeltaForHigherMargin)
{
    ratings::MatchStorage service;
    RegisterPlayers(service, {"Alice", "Bob", "Carol", "Dave"});

    EXPECT_TRUE(service.RecordMatch(BuildMatchResult(8, 5)));

    EXPECT_EQ(service.GetPlayer("Alice")->GetElo(), kEloAfterHighMarginWin);
    EXPECT_EQ(service.GetPlayer("Bob")->GetElo(), kEloAfterHighMarginWin);
    EXPECT_EQ(service.GetPlayer("Carol")->GetElo(), kEloAfterHighMarginLoss);
    EXPECT_EQ(service.GetPlayer("Dave")->GetElo(), kEloAfterHighMarginLoss);
}

TEST(MatchStorageTest, StoresMatchHistory)
{
    ratings::MatchStorage service;
    RegisterPlayers(service, {"Alice", "Bob", "Carol", "Dave"});

    constexpr std::uint8_t kFirstTeamGoals{7};
    constexpr std::uint8_t kSecondTeamGoals{3};
    EXPECT_TRUE(service.RecordMatch(BuildMatchResult(kFirstTeamGoals, kSecondTeamGoals)));

    ASSERT_EQ(service.GetMatchHistory().size(), 1U);
    const auto& match = service.GetMatchHistory().front();

    EXPECT_EQ(match.teams_.at(0).players.first.GetNickname(), "Alice");
    EXPECT_EQ(match.teams_.at(0).players.second.GetNickname(), "Bob");
    EXPECT_EQ(match.teams_.at(1).players.first.GetNickname(), "Carol");
    EXPECT_EQ(match.teams_.at(1).players.second.GetNickname(), "Dave");
    EXPECT_EQ(match.set_scores_.at(0).at(0), kFirstTeamGoals);
    EXPECT_EQ(match.set_scores_.at(1).at(0), kSecondTeamGoals);
}

TEST(MatchStorageTest, EloChangeDependsOnOpponentStrength)
{
    const auto gain_vs_strong = AliceEloDeltaAfterMatch(kStrongOpponents, 8, 6);
    const auto gain_vs_weak = AliceEloDeltaAfterMatch(kWeakOpponents, 8, 6);
    const auto loss_vs_strong = -AliceEloDeltaAfterMatch(kStrongOpponents, 8, 10);
    const auto loss_vs_weak = -AliceEloDeltaAfterMatch(kWeakOpponents, 8, 10);

    EXPECT_GT(gain_vs_strong, gain_vs_weak);
    EXPECT_GT(loss_vs_weak, loss_vs_strong);
}

TEST(MatchStorageTest, RejectsMatchWhenAnyPlayerIsNotRegistered)
{
    ratings::MatchStorage service;
    RegisterPlayers(service, {"Alice", "Bob", "Carol"});

    EXPECT_FALSE(service.RecordMatch(BuildMatchResult(8, 6)));
    EXPECT_TRUE(service.GetMatchHistory().empty());
}
