#include <gtest/gtest.h>

#include <cstdint>
#include <filesystem>
#include <stdexcept>

#include "sql_connection.hpp"

const std::filesystem::path kStatementTestDbPath{"/tmp/foosball_statement_test.db"};

class SqlStatementTest : public ::testing::Test
{
  protected:
    void SetUp() override
    {
        std::filesystem::remove(kStatementTestDbPath);
        connection_ = std::make_unique<db::SqlConnection>(kStatementTestDbPath.string());
        connection_->TryExecuteSql(
          "CREATE TABLE t (id INTEGER PRIMARY KEY AUTOINCREMENT, name TEXT, score INTEGER, "
          "big INTEGER)");
    }

    void TearDown() override
    {
        connection_.reset();
        std::filesystem::remove(kStatementTestDbPath);
    }

    std::unique_ptr<db::SqlConnection> connection_;
};

TEST_F(SqlStatementTest, InvalidStatementIsNotValid)
{
    EXPECT_THROW((void)connection_->CreateStatement("SELECT * FROM nonexistent"),
                 std::runtime_error);
}

TEST_F(SqlStatementTest, BindTextAndFetchRow)
{
    auto insert{connection_->CreateStatement("INSERT INTO t (name, score, big) VALUES (?, ?, ?)")};
    ASSERT_TRUE(insert.TryBindText(1, "Alice"));
    ASSERT_TRUE(insert.TryBindInt(2, 42));
    ASSERT_TRUE(insert.TryBindInt64(3, 9999999999LL));
    EXPECT_TRUE(insert.TryExecute());

    auto select{connection_->CreateStatement("SELECT name, score, big FROM t WHERE name = ?")};
    ASSERT_TRUE(select.TryBindText(1, "Alice"));
    ASSERT_TRUE(select.TryFetchRow());
    EXPECT_EQ(select.GetColumnString(0), "Alice");
    EXPECT_EQ(select.GetColumnInt(1), 42);
    EXPECT_EQ(select.GetColumnInt64(2), 9999999999LL);
}

TEST_F(SqlStatementTest, FetchRowReturnsFalseWhenNoRows)
{
    auto select{connection_->CreateStatement("SELECT * FROM t")};
    EXPECT_FALSE(select.TryFetchRow());
}

TEST_F(SqlStatementTest, ExecuteReturnsFalseOnInvalidStatement)
{
    EXPECT_THROW((void)connection_->CreateStatement("SELECT * FROM nonexistent"),
                 std::runtime_error);
}

TEST_F(SqlStatementTest, FetchRowReturnsFalseOnInvalidStatement)
{
    EXPECT_THROW((void)connection_->CreateStatement("SELECT * FROM nonexistent"),
                 std::runtime_error);
}

TEST_F(SqlStatementTest, ColumnStringReturnsEmptyOnInvalidStatement)
{
    EXPECT_THROW((void)connection_->CreateStatement("SELECT * FROM nonexistent"),
                 std::runtime_error);
}

TEST_F(SqlStatementTest, ColumnIntReturnsZeroOnInvalidStatement)
{
    EXPECT_THROW((void)connection_->CreateStatement("SELECT * FROM nonexistent"),
                 std::runtime_error);
}

TEST_F(SqlStatementTest, ColumnInt64ReturnsZeroOnInvalidStatement)
{
    EXPECT_THROW((void)connection_->CreateStatement("SELECT * FROM nonexistent"),
                 std::runtime_error);
}
