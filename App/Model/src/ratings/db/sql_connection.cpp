#include "sql_connection.hpp"

#include <spdlog/spdlog.h>
#include <sqlite3.h>

#include <stdexcept>
#include <string>
#include <tuple>

namespace db
{
SqlConnection::SqlConnection(const std::string_view& path)
{
    if (sqlite3_open(std::string{path}.c_str(), &db_) != SQLITE_OK)
    {
        sqlite3_close(db_);
        spdlog::error("SqlConnection: cannot open '{}': {}", path, sqlite3_errmsg(db_));
        throw std::runtime_error{"SqlConnection: cannot open database"};
    }
}

SqlConnection::~SqlConnection() { sqlite3_close(db_); }

void SqlConnection::ExecuteSql(const std::string_view sql) { std::ignore = TryExecuteSql(sql); }

bool SqlConnection::TryExecuteSql(const std::string_view sql)
{
    const std::string sql_text{sql};
    char* err{nullptr};
    if (sqlite3_exec(db_, sql_text.c_str(), nullptr, nullptr, &err) != SQLITE_OK)
    {
        spdlog::error("SqlConnection::TryExecuteSql: {}", err);
        sqlite3_free(err);
        return false;
    }
    return true;
}

SqlStatement SqlConnection::CreateStatement(const std::string_view sql) const
{
    sqlite3_stmt* ptr{nullptr};
    if (sqlite3_prepare_v2(db_, sql.data(), static_cast<int>(sql.size()), &ptr, nullptr) !=
        SQLITE_OK)
    {
        spdlog::error("SqlConnection::CreateStatement: {}", sqlite3_errmsg(db_));
        throw std::runtime_error{"SqlConnection::CreateStatement failed"};
    }
    return SqlStatement{ptr};
}

int SqlConnection::GetAffectedRows() const { return sqlite3_changes(db_); }

std::int64_t SqlConnection::LastInsertRowid() const { return sqlite3_last_insert_rowid(db_); }

}  // namespace db
