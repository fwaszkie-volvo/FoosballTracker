#include "sql_statement.hpp"

#include <sqlite3.h>

#include <string>

namespace db
{
SqlStatement::~SqlStatement() { sqlite3_finalize(statement_); }

SqlStatement::SqlStatement(SqlStatement&& other) noexcept : statement_{other.statement_}
{
    other.statement_ = nullptr;
}

bool SqlStatement::TryBindText(const int idx, const std::string_view value)
{
    return sqlite3_bind_text(
             statement_, idx, value.data(), static_cast<int>(value.size()), SQLITE_TRANSIENT) ==
           SQLITE_OK;
}

bool SqlStatement::TryBindInt(const int idx, const int value)
{
    return sqlite3_bind_int(statement_, idx, value) == SQLITE_OK;
}

bool SqlStatement::TryBindInt64(const int idx, const std::int64_t value)
{
    return sqlite3_bind_int64(statement_, idx, value) == SQLITE_OK;
}

bool SqlStatement::TryFetchRow() { return sqlite3_step(statement_) == SQLITE_ROW; }

bool SqlStatement::TryExecute() { return sqlite3_step(statement_) == SQLITE_DONE; }

std::string SqlStatement::GetColumnString(const int column_index) const
{
    const void* raw_value{sqlite3_column_text(statement_, column_index)};
    return std::string{static_cast<const char*>(raw_value)};
}

int SqlStatement::GetColumnInt(const int column_index) const
{
    return sqlite3_column_int(statement_, column_index);
}

std::int64_t SqlStatement::GetColumnInt64(const int column_index) const
{
    return sqlite3_column_int64(statement_, column_index);
}
}  // namespace db
