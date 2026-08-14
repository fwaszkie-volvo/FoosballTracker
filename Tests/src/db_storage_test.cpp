#include "db_storage.hpp"

#include <gtest/gtest.h>

#include <cstdint>
#include <filesystem>

#include "model_types.hpp"
#include "player.hpp"
#include "ratings_types.hpp"

constexpr int kInitialElo{1000};
constexpr int kUpdatedElo{1050};
constexpr int kWinnerElo{1019};
constexpr int kLoserElo{981};

const std::filesystem::path kTestDbPath{"foosball_tracker.db"};

class DbStorageTest : public ::testing::Test
{
  protected:
    void SetUp() override
    {
        std::filesystem::remove(kTestDbPath);
        storage_ = std::make_unique<db::DbStorage>();
    }

    void TearDown() override
    {
        storage_.reset();
        std::filesystem::remove(kTestDbPath);
    }

    static ratings::MatchInput BuildMatch(std::uint8_t first_goals, std::uint8_t second_goals)
    {
        const model::Teams teams{model::Team{{Player{"Alice"}, Player{"Bob"}}},
                                 model::Team{{Player{"Carol"}, Player{"Dave"}}}};
        const ratings::TeamSettings team_settings{teams, teams, teams, teams};
        ratings::MatchInput match{.teams_ = teams, .team_settings_ = team_settings};
        for (std::size_t s{0}; s < model::kSetsPerMatch; ++s)
        {
            match.set_scores_.at(0).at(s) = first_goals;
            match.set_scores_.at(1).at(s) = second_goals;
        }
        return match;
    }

    void CreateMatchPlayers() const
    {
        storage_->CreatePlayer("Alice");
        storage_->CreatePlayer("Bob");
        storage_->CreatePlayer("Carol");
        storage_->CreatePlayer("Dave");
    }

    std::unique_ptr<db::DbStorage> storage_;
};

TEST_F(DbStorageTest, CreateAndGetPlayer)
{
    storage_->CreatePlayer("Alice");

    const auto player = storage_->GetPlayer("Alice");
    ASSERT_TRUE(player.has_value());
    EXPECT_EQ(player->GetNickname(), "Alice");
    EXPECT_EQ(player->GetElo(), kInitialElo);
}

TEST_F(DbStorageTest, CreatePlayerIsIdempotentForExistingNickname)
{
    storage_->CreatePlayer("Alice");
    storage_->CreatePlayer("Alice");

    const auto player = storage_->GetPlayer("Alice");
    ASSERT_TRUE(player.has_value());
    EXPECT_EQ(player->GetElo(), kInitialElo);
}

TEST_F(DbStorageTest, GetPlayerReturnsNulloptForUnknown)
{
    EXPECT_FALSE(storage_->GetPlayer("Nobody").has_value());
}

TEST_F(DbStorageTest, UpdatePlayerElo)
{
    storage_->CreatePlayer("Alice");
    ASSERT_TRUE(storage_->TryUpdatePlayerElo("Alice", kUpdatedElo));

    const auto player = storage_->GetPlayer("Alice");
    ASSERT_TRUE(player.has_value());
    EXPECT_EQ(player->GetElo(), kUpdatedElo);
}

TEST_F(DbStorageTest, UpdatePlayerEloReturnsFalseForUnknown)
{
    EXPECT_FALSE(storage_->TryUpdatePlayerElo("Nobody", kUpdatedElo));
}

TEST_F(DbStorageTest, RecordAndRetrieveMatch)
{
    CreateMatchPlayers();
    ASSERT_TRUE(storage_->InsertMatch(
      BuildMatch(3, 1),
      model::PlayerEloMap{
        {"Alice", kWinnerElo}, {"Bob", kWinnerElo}, {"Carol", kLoserElo}, {"Dave", kLoserElo}}));

    const auto history = storage_->GetMatchHistory();
    ASSERT_EQ(history.size(), 1U);

    const auto& match = history.front();
    EXPECT_EQ(match.teams_.first.players.first.GetNickname(), "Alice");
    EXPECT_EQ(match.teams_.first.players.second.GetNickname(), "Bob");
    EXPECT_EQ(match.teams_.second.players.first.GetNickname(), "Carol");
    EXPECT_EQ(match.teams_.second.players.second.GetNickname(), "Dave");

    for (std::size_t s{0}; s < model::kSetsPerMatch; ++s)
    {
        EXPECT_EQ(match.set_scores_.at(0).at(s), 3);
        EXPECT_EQ(match.set_scores_.at(1).at(s), 1);
    }

    for (std::size_t s{0}; s < model::kSetsPerMatch; ++s)
    {
        EXPECT_EQ(match.team_settings_.at(s).first.players.first.GetNickname(), "Alice");
        EXPECT_EQ(match.team_settings_.at(s).first.players.second.GetNickname(), "Bob");
        EXPECT_EQ(match.team_settings_.at(s).second.players.first.GetNickname(), "Carol");
        EXPECT_EQ(match.team_settings_.at(s).second.players.second.GetNickname(), "Dave");
    }
}

TEST_F(DbStorageTest, MultipleMatchesPreserveOrder)
{
    CreateMatchPlayers();
    ASSERT_TRUE(storage_->InsertMatch(
      BuildMatch(3, 0),
      model::PlayerEloMap{
        {"Alice", kWinnerElo}, {"Bob", kWinnerElo}, {"Carol", kLoserElo}, {"Dave", kLoserElo}}));
    ASSERT_TRUE(storage_->InsertMatch(
      BuildMatch(1, 2),
      model::PlayerEloMap{
        {"Alice", kLoserElo}, {"Bob", kLoserElo}, {"Carol", kWinnerElo}, {"Dave", kWinnerElo}}));

    const auto history = storage_->GetMatchHistory();
    ASSERT_EQ(history.size(), 2U);
    EXPECT_EQ(history.at(0).set_scores_.at(0).at(0), 3);
    EXPECT_EQ(history.at(1).set_scores_.at(0).at(0), 1);
}

TEST_F(DbStorageTest, EmptyHistoryOnFreshDb) { EXPECT_TRUE(storage_->GetMatchHistory().empty()); }

TEST_F(DbStorageTest, RecordMatchDoesNothingWhenPlayerIsMissing)
{
    storage_->CreatePlayer("Alice");
    storage_->CreatePlayer("Bob");

    EXPECT_FALSE(storage_->InsertMatch(
      BuildMatch(3, 1),
      model::PlayerEloMap{
        {"Alice", kWinnerElo}, {"Bob", kWinnerElo}, {"Carol", kLoserElo}, {"Dave", kLoserElo}}));
    EXPECT_TRUE(storage_->GetMatchHistory().empty());
}

TEST_F(DbStorageTest, RecordMatchUpdatesPlayersElo)
{
    CreateMatchPlayers();

    ASSERT_TRUE(storage_->InsertMatch(
      BuildMatch(8, 5),
      model::PlayerEloMap{
        {"Alice", kWinnerElo}, {"Bob", kWinnerElo}, {"Carol", kLoserElo}, {"Dave", kLoserElo}}));

    const auto alice = storage_->GetPlayer("Alice");
    const auto bob = storage_->GetPlayer("Bob");
    const auto carol = storage_->GetPlayer("Carol");
    const auto dave = storage_->GetPlayer("Dave");

    ASSERT_TRUE(alice.has_value());
    ASSERT_TRUE(bob.has_value());
    ASSERT_TRUE(carol.has_value());
    ASSERT_TRUE(dave.has_value());
    EXPECT_EQ(alice->GetElo(), 1019);
    EXPECT_EQ(bob->GetElo(), 1019);
    EXPECT_EQ(carol->GetElo(), 981);
    EXPECT_EQ(dave->GetElo(), 981);
}

TEST_F(DbStorageTest, PersistsAcrossReopens)
{
    CreateMatchPlayers();
    ASSERT_TRUE(storage_->InsertMatch(
      BuildMatch(2, 1),
      model::PlayerEloMap{
        {"Alice", kWinnerElo}, {"Bob", kWinnerElo}, {"Carol", kLoserElo}, {"Dave", kLoserElo}}));

    const auto player_before_reopen = storage_->GetPlayer("Alice");
    ASSERT_TRUE(player_before_reopen.has_value());

    storage_.reset();

    db::DbStorage reopened{};
    const auto player = reopened.GetPlayer("Alice");
    ASSERT_TRUE(player.has_value());
    EXPECT_EQ(player->GetElo(), player_before_reopen->GetElo());
    EXPECT_EQ(reopened.GetMatchHistory().size(), 1U);
}
