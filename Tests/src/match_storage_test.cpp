#include "match_storage.hpp"

#include <gtest/gtest.h>

#include <initializer_list>
#include <utility>

static constexpr int kInitialElo{1000};
static constexpr int kEloAfterWin{1018};
static constexpr int kEloAfterLoss{982};
static constexpr int kEloAfterHighMarginWin{1019};
static constexpr int kEloAfterHighMarginLoss{981};

static constexpr bool kStrongOpponents{true};
static constexpr bool kWeakOpponents{false};

static constexpr int kPreparationMatches{8};

class MatchStorageTest : public ::testing::Test
{
  protected:
    void RegisterPlayers(const std::initializer_list<model::Nickname>& nicknames)
    {
        for (const auto& nickname : nicknames)
        {
            storage_.CreatePlayer(nickname);
        }
    }

    static ratings::MatchInput BuildMatchResult(const std::uint8_t first_team_goals,
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

    void PrepareOpponents(const bool strong_opponents)
    {
        RegisterPlayers({"Carol", "Dave", "Eve", "Frank"});

        for (int index{0}; index < kPreparationMatches; ++index)
        {
            EXPECT_TRUE(storage_.RecordMatch(ratings::MatchInput{
              .teams_ = strong_opponents
                          ? model::Teams{model::Team{.players = {Player{"Carol"}, Player{"Dave"}}},
                                         model::Team{.players = {Player{"Eve"}, Player{"Frank"}}}}
                          : model::Teams{model::Team{.players = {Player{"Eve"}, Player{"Frank"}}},
                                         model::Team{.players = {Player{"Carol"}, Player{"Dave"}}}},
              .set_scores_ = {{{8, 8, 8, 8}, {0, 0, 0, 0}}},
            }));
        }
    }

    int AliceEloDeltaAfterMatch(const bool strong_opponents,
                                const std::uint8_t alice_team_goals,
                                const std::uint8_t opponents_goals)
    {
        storage_ = ratings::MatchStorage{};
        RegisterPlayers({"Alice", "Bob", "Carol", "Dave"});
        PrepareOpponents(strong_opponents);
        EXPECT_TRUE(storage_.RecordMatch(BuildMatchResult(alice_team_goals, opponents_goals)));

        const auto alice = storage_.GetPlayer("Alice");
        EXPECT_TRUE(alice.has_value());
        if (!alice.has_value())
        {
            return 0;
        }

        return alice->GetElo() - kInitialElo;
    }

    ratings::MatchStorage storage_{};
};

TEST_F(MatchStorageTest, CreatesPlayersWithDefaultInitialElo)
{
    storage_.CreatePlayer("Alice");

    const auto alice = storage_.GetPlayer("Alice");

    ASSERT_TRUE(alice.has_value());
    EXPECT_EQ(alice->GetNickname(), "Alice");
    EXPECT_EQ(alice->GetElo(), kInitialElo);
}

TEST_F(MatchStorageTest, UpdatesEloForWinInTwoVsTwoMatch)
{
    RegisterPlayers({"Alice", "Bob", "Carol", "Dave"});

    EXPECT_TRUE(storage_.RecordMatch(BuildMatchResult(8, 6)));

    const auto alice = storage_.GetPlayer("Alice");
    const auto bob = storage_.GetPlayer("Bob");
    const auto carol = storage_.GetPlayer("Carol");
    const auto dave = storage_.GetPlayer("Dave");

    ASSERT_TRUE(alice.has_value());
    ASSERT_TRUE(bob.has_value());
    ASSERT_TRUE(carol.has_value());
    ASSERT_TRUE(dave.has_value());

    EXPECT_EQ(alice->GetElo(), kEloAfterWin);
    EXPECT_EQ(bob->GetElo(), kEloAfterWin);
    EXPECT_EQ(carol->GetElo(), kEloAfterLoss);
    EXPECT_EQ(dave->GetElo(), kEloAfterLoss);
}

TEST_F(MatchStorageTest, DoesNotChangeRatingsForDrawWithEqualTeams)
{
    RegisterPlayers({"Alice", "Bob", "Carol", "Dave"});

    EXPECT_TRUE(storage_.RecordMatch(BuildMatchResult(5, 5)));

    EXPECT_EQ(storage_.GetPlayer("Alice")->GetElo(), kInitialElo);
    EXPECT_EQ(storage_.GetPlayer("Bob")->GetElo(), kInitialElo);
    EXPECT_EQ(storage_.GetPlayer("Carol")->GetElo(), kInitialElo);
    EXPECT_EQ(storage_.GetPlayer("Dave")->GetElo(), kInitialElo);
}

TEST_F(MatchStorageTest, GoalDifferenceIncreasesEloDeltaForHigherMargin)
{
    RegisterPlayers({"Alice", "Bob", "Carol", "Dave"});

    EXPECT_TRUE(storage_.RecordMatch(BuildMatchResult(8, 5)));

    EXPECT_EQ(storage_.GetPlayer("Alice")->GetElo(), kEloAfterHighMarginWin);
    EXPECT_EQ(storage_.GetPlayer("Bob")->GetElo(), kEloAfterHighMarginWin);
    EXPECT_EQ(storage_.GetPlayer("Carol")->GetElo(), kEloAfterHighMarginLoss);
    EXPECT_EQ(storage_.GetPlayer("Dave")->GetElo(), kEloAfterHighMarginLoss);
}

TEST_F(MatchStorageTest, StoresMatchHistory)
{
    RegisterPlayers({"Alice", "Bob", "Carol", "Dave"});

    constexpr std::uint8_t kFirstTeamGoals{7};
    constexpr std::uint8_t kSecondTeamGoals{3};
    EXPECT_TRUE(storage_.RecordMatch(BuildMatchResult(kFirstTeamGoals, kSecondTeamGoals)));

    ASSERT_EQ(storage_.GetMatchHistory().size(), 1U);
    const auto& match = storage_.GetMatchHistory().front();

    EXPECT_EQ(match.teams_.first.players.first.GetNickname(), "Alice");
    EXPECT_EQ(match.teams_.first.players.second.GetNickname(), "Bob");
    EXPECT_EQ(match.teams_.second.players.first.GetNickname(), "Carol");
    EXPECT_EQ(match.teams_.second.players.second.GetNickname(), "Dave");
    EXPECT_EQ(match.set_scores_.at(0).at(0), kFirstTeamGoals);
    EXPECT_EQ(match.set_scores_.at(1).at(0), kSecondTeamGoals);
}

TEST_F(MatchStorageTest, EloChangeDependsOnOpponentStrength)
{
    const auto gain_vs_strong = AliceEloDeltaAfterMatch(kStrongOpponents, 8, 6);
    const auto gain_vs_weak = AliceEloDeltaAfterMatch(kWeakOpponents, 8, 6);
    const auto loss_vs_strong = -AliceEloDeltaAfterMatch(kStrongOpponents, 8, 10);
    const auto loss_vs_weak = -AliceEloDeltaAfterMatch(kWeakOpponents, 8, 10);

    EXPECT_GT(gain_vs_strong, gain_vs_weak);
    EXPECT_GT(loss_vs_weak, loss_vs_strong);
}

TEST_F(MatchStorageTest, RejectsMatchWhenAnyPlayerIsNotRegistered)
{
    RegisterPlayers({"Alice", "Bob", "Carol"});

    EXPECT_FALSE(storage_.RecordMatch(BuildMatchResult(8, 6)));
    EXPECT_TRUE(storage_.GetMatchHistory().empty());
}
