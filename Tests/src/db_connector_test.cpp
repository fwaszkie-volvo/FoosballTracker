#include <gtest/gtest.h>

#include <filesystem>
#include <stdexcept>

#include "sql_connection.hpp"

const std::filesystem::path kConnectorTestDbPath{"/tmp/foosball_connector_test.db"};

class DbConnectorTest : public ::testing::Test
{
  protected:
    void SetUp() override
    {
        std::filesystem::remove(kConnectorTestDbPath);
        connection_ = std::make_unique<db::SqlConnection>(kConnectorTestDbPath.string());
    }

    void TearDown() override
    {
        connection_.reset();
        std::filesystem::remove(kConnectorTestDbPath);
    }

    std::unique_ptr<db::SqlConnection> connection_;
};

TEST_F(DbConnectorTest, FailsOnInvalidPath)
{
    EXPECT_THROW((void)db::SqlConnection{"/no/such/dir/test.db"}, std::runtime_error);
}

TEST_F(DbConnectorTest, ExecCreatesTable)
{
    EXPECT_TRUE(connection_->TryExecuteSql("CREATE TABLE t (id INTEGER PRIMARY KEY)"));
}

TEST_F(DbConnectorTest, ExecReturnsFalseOnInvalidSql)
{
    EXPECT_FALSE(connection_->TryExecuteSql("NOT VALID SQL"));
}

TEST_F(DbConnectorTest, PrepareValidStatement)
{
    connection_->TryExecuteSql("CREATE TABLE t (id INTEGER PRIMARY KEY, val TEXT)");
    auto stmt{connection_->CreateStatement("SELECT id, val FROM t")};
    EXPECT_FALSE(stmt.TryFetchRow());
}

TEST_F(DbConnectorTest, PrepareInvalidStatementIsNotValid)
{
    EXPECT_THROW((void)connection_->CreateStatement("SELECT * FROM nonexistent_table"),
                 std::runtime_error);
}

TEST_F(DbConnectorTest, ChangesAfterInsert)
{
    connection_->TryExecuteSql("CREATE TABLE t (id INTEGER PRIMARY KEY)");
    auto stmt{connection_->CreateStatement("INSERT INTO t VALUES (?)")};
    stmt.TryBindInt(1, 42);
    stmt.TryExecute();
    EXPECT_EQ(connection_->GetAffectedRows(), 1);
}

TEST_F(DbConnectorTest, LastInsertRowidAfterInsert)
{
    connection_->TryExecuteSql("CREATE TABLE t (id INTEGER PRIMARY KEY AUTOINCREMENT, val TEXT)");
    auto stmt{connection_->CreateStatement("INSERT INTO t (val) VALUES (?)")};
    stmt.TryBindText(1, "hello");
    stmt.TryExecute();
    EXPECT_EQ(connection_->LastInsertRowid(), 1);
}

TEST_F(DbConnectorTest, TransactionCommit)
{
    connection_->TryExecuteSql("CREATE TABLE t (id INTEGER PRIMARY KEY)");
    EXPECT_TRUE(connection_->TryExecuteSql("BEGIN"));
    auto stmt{connection_->CreateStatement("INSERT INTO t VALUES (?)")};
    stmt.TryBindInt(1, 1);
    stmt.TryExecute();
    EXPECT_TRUE(connection_->TryExecuteSql("COMMIT"));
    EXPECT_EQ(connection_->GetAffectedRows(), 1);
}

TEST_F(DbConnectorTest, TransactionRollback)
{
    connection_->TryExecuteSql("CREATE TABLE t (id INTEGER PRIMARY KEY)");
    connection_->TryExecuteSql("BEGIN");
    auto stmt{connection_->CreateStatement("INSERT INTO t VALUES (?)")};
    stmt.TryBindInt(1, 1);
    stmt.TryExecute();
    EXPECT_TRUE(connection_->TryExecuteSql("ROLLBACK"));

    auto check{connection_->CreateStatement("SELECT COUNT(*) FROM t")};
    ASSERT_TRUE(check.TryFetchRow());
    EXPECT_EQ(check.GetColumnInt(0), 0);
}
