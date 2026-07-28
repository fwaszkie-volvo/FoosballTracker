#include "player_ratings.hpp"

#include <gtest/gtest.h>

static constexpr int kInitialElo{1000};
static constexpr int kEloAfterWin{1016};
static constexpr int kEloAfterLoss{984};

static constexpr bool kStrongOpponents{true};
static constexpr bool kWeakOpponents{false};

namespace
{
constexpr int kPreparationMatches{8};

ratings::MatchInput BuildMatchResult(const std::uint8_t first_team_goals,
                                     const std::uint8_t second_team_goals)
{
    return ratings::MatchInput{
      .nicknames = {{{"Alice", "Bob"}, {"Carol", "Dave"}}},
      .set_scores =
        {{{first_team_goals, first_team_goals, first_team_goals, first_team_goals},
          {second_team_goals, second_team_goals, second_team_goals, second_team_goals}}},
    };
}

void PrepareOpponentsRatings(ratings::PlayerRatingsService& service, const bool strong_opponents)
{
    for (int index{0}; index < kPreparationMatches; ++index)
    {
        service.RecordMatch(ratings::MatchInput{
          .nicknames = strong_opponents
                         ? ratings::PlayersNicknames{{{"Carol", "Dave"}, {"Eve", "Frank"}}}
                         : ratings::PlayersNicknames{{{"Eve", "Frank"}, {"Carol", "Dave"}}},
          .set_scores = {{{8, 8, 8, 8}, {0, 0, 0, 0}}},
        });
    }
}

int AliceEloDeltaAfterMatch(const bool strong_opponents,
                            const std::uint8_t alice_team_goals,
                            const std::uint8_t opponents_goals)
{
    ratings::PlayerRatingsService service;
    PrepareOpponentsRatings(service, strong_opponents);
    service.RecordMatch(BuildMatchResult(alice_team_goals, opponents_goals));

    const auto alice = service.GetPlayer("Alice");
    EXPECT_TRUE(alice.has_value());
    if (!alice.has_value())
    {
        return 0;
    }

    return alice->GetElo() - kInitialElo;
}
}  // namespace

TEST(PlayerRatingsServiceTest, CreatesPlayersWithDefaultInitialElo)
{
    ratings::PlayerRatingsService service;

    service.RecordMatch(BuildMatchResult(0, 0));

    const auto alice = service.GetPlayer("Alice");

    ASSERT_TRUE(alice.has_value());
    EXPECT_EQ(alice->GetNickname(), "Alice");
    EXPECT_EQ(alice->GetElo(), kInitialElo);
}

TEST(PlayerRatingsServiceTest, UpdatesEloForWinInTwoVsTwoMatch)
{
    ratings::PlayerRatingsService service;

    service.RecordMatch(BuildMatchResult(8, 6));

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

TEST(PlayerRatingsServiceTest, DoesNotChangeRatingsForDrawWithEqualTeams)
{
    ratings::PlayerRatingsService service;

    service.RecordMatch(BuildMatchResult(5, 5));

    EXPECT_EQ(service.GetPlayer("Alice")->GetElo(), kInitialElo);
    EXPECT_EQ(service.GetPlayer("Bob")->GetElo(), kInitialElo);
    EXPECT_EQ(service.GetPlayer("Carol")->GetElo(), kInitialElo);
    EXPECT_EQ(service.GetPlayer("Dave")->GetElo(), kInitialElo);
}

TEST(PlayerRatingsServiceTest, GoalDifferenceIsDisabledByDefault)
{
    ratings::PlayerRatingsService service;

    service.RecordMatch(BuildMatchResult(8, 5));

    EXPECT_EQ(service.GetPlayer("Alice")->GetElo(), kEloAfterWin);
    EXPECT_EQ(service.GetPlayer("Bob")->GetElo(), kEloAfterWin);
    EXPECT_EQ(service.GetPlayer("Carol")->GetElo(), kEloAfterLoss);
    EXPECT_EQ(service.GetPlayer("Dave")->GetElo(), kEloAfterLoss);
}

TEST(PlayerRatingsServiceTest, StoresMatchHistory)
{
    ratings::PlayerRatingsService service;

    constexpr std::uint8_t kFirstTeamGoals{7};
    constexpr std::uint8_t kSecondTeamGoals{3};
    service.RecordMatch(BuildMatchResult(kFirstTeamGoals, kSecondTeamGoals));

    ASSERT_EQ(service.GetMatchHistory().size(), 1U);
    const auto& match = service.GetMatchHistory().front();

    EXPECT_EQ(match.nicknames.at(0).at(0), "Alice");
    EXPECT_EQ(match.nicknames.at(0).at(1), "Bob");
    EXPECT_EQ(match.nicknames.at(1).at(0), "Carol");
    EXPECT_EQ(match.nicknames.at(1).at(1), "Dave");
    EXPECT_EQ(match.set_scores.at(0).at(0), kFirstTeamGoals);
    EXPECT_EQ(match.set_scores.at(1).at(0), kSecondTeamGoals);
}

TEST(PlayerRatingsServiceTest, EloChangeDependsOnOpponentStrength)
{
    const auto gain_vs_strong = AliceEloDeltaAfterMatch(kStrongOpponents, 8, 6);
    const auto gain_vs_weak = AliceEloDeltaAfterMatch(kWeakOpponents, 8, 6);
    const auto loss_vs_strong = -AliceEloDeltaAfterMatch(kStrongOpponents, 8, 10);
    const auto loss_vs_weak = -AliceEloDeltaAfterMatch(kWeakOpponents, 8, 10);

    EXPECT_GT(gain_vs_strong, gain_vs_weak);
    EXPECT_GT(loss_vs_weak, loss_vs_strong);
}
