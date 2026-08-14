#ifndef FOOSBALL_TRACKER_APP_MODEL_INCLUDE_PRIVATE_RATINGS_DB_DB_SCHEMA_HPP_
#define FOOSBALL_TRACKER_APP_MODEL_INCLUDE_PRIVATE_RATINGS_DB_DB_SCHEMA_HPP_

#include <string_view>

namespace db
{
constexpr std::string_view kSchema{R"sql(
    PRAGMA journal_mode = WAL;

    CREATE TABLE IF NOT EXISTS players (
        nickname TEXT PRIMARY KEY,
        elo      INTEGER NOT NULL
    );

    CREATE TABLE IF NOT EXISTS matches (
        id       INTEGER PRIMARY KEY AUTOINCREMENT,
        recorded TEXT DEFAULT (datetime('now'))
    );

    CREATE TABLE IF NOT EXISTS match_teams (
        match_id   INTEGER NOT NULL REFERENCES matches(id),
        team_index INTEGER NOT NULL,
        player1    TEXT NOT NULL,
        player2    TEXT NOT NULL,
        PRIMARY KEY (match_id, team_index)
    );

    CREATE TABLE IF NOT EXISTS set_scores (
        match_id   INTEGER NOT NULL REFERENCES matches(id),
        team_index INTEGER NOT NULL,
        set_index  INTEGER NOT NULL,
        goals      INTEGER NOT NULL,
        PRIMARY KEY (match_id, team_index, set_index)
    );

    CREATE TABLE IF NOT EXISTS team_settings (
        match_id      INTEGER NOT NULL REFERENCES matches(id),
        setting_index INTEGER NOT NULL,
        team_index    INTEGER NOT NULL,
        player1       TEXT NOT NULL,
        player2       TEXT NOT NULL,
        PRIMARY KEY (match_id, setting_index, team_index)
    );
    )sql"};
}  // namespace db

#endif  // FOOSBALL_TRACKER_APP_MODEL_INCLUDE_PRIVATE_RATINGS_DB_DB_SCHEMA_HPP_
