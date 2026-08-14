#ifndef FOOSBALL_TRACKER_APP_MODEL_INCLUDE_PRIVATE_RATINGS_DB_SQL_CONNECTION_HPP_
#define FOOSBALL_TRACKER_APP_MODEL_INCLUDE_PRIVATE_RATINGS_DB_SQL_CONNECTION_HPP_

#include <sqlite3.h>

#include <cstdint>
#include <string_view>

#include "sql_statement.hpp"

namespace db
{
class SqlConnection
{
  public:
    explicit SqlConnection(const std::string_view& path);
    ~SqlConnection();
    SqlConnection(const SqlConnection&)            = delete;
    SqlConnection& operator=(const SqlConnection&) = delete;

    void ExecuteSql(const std::string_view sql);
    bool TryExecuteSql(const std::string_view sql);
    SqlStatement CreateStatement(const std::string_view sql) const;

    int GetAffectedRows() const;
    std::int64_t LastInsertRowid() const;

  private:
    sqlite3* db_{nullptr};
};
}  // namespace db

#endif  // FOOSBALL_TRACKER_APP_MODEL_INCLUDE_PRIVATE_RATINGS_DB_SQL_CONNECTION_HPP_
