#ifndef FOOSBALL_TRACKER_APP_MODEL_INCLUDE_PRIVATE_RATINGS_DB_SQL_STATEMENT_HPP_
#define FOOSBALL_TRACKER_APP_MODEL_INCLUDE_PRIVATE_RATINGS_DB_SQL_STATEMENT_HPP_

#include <sqlite3.h>

#include <cstdint>
#include <string>
#include <string_view>

namespace db
{
class SqlStatement
{
  public:
    SqlStatement(sqlite3_stmt* sqlite_statement_pointer) : statement_{sqlite_statement_pointer} {}

    ~SqlStatement();
    SqlStatement(const SqlStatement&)            = delete;
    SqlStatement& operator=(const SqlStatement&) = delete;
    SqlStatement(SqlStatement&& other) noexcept;

    bool TryBindText(const int idx, const std::string_view value);
    bool TryBindInt(const int idx, const int value);
    bool TryBindInt64(const int idx, const std::int64_t value);

    bool TryFetchRow();
    bool TryExecute();

    std::string GetColumnString(const int column_index) const;
    int GetColumnInt(const int column_index) const;
    std::int64_t GetColumnInt64(const int column_index) const;

  private:
    sqlite3_stmt* statement_{};
};
}  // namespace db

#endif  // FOOSBALL_TRACKER_APP_MODEL_INCLUDE_PRIVATE_RATINGS_DB_SQL_STATEMENT_HPP_
