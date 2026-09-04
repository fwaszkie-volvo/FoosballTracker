#ifndef FOOSBALL_TRACKER_APP_MODEL_INCLUDE_PRIVATE_PLAYER_HPP_
#define FOOSBALL_TRACKER_APP_MODEL_INCLUDE_PRIVATE_PLAYER_HPP_

#include <string>
#include <utility>

#include "common_types.hpp"

static constexpr int kDefaultElo{1000};

class Player
{
  public:
    Player(common::Nickname nickname, const int elo = kDefaultElo)
        : nickname_(std::move(nickname)), elo_(elo)
    {
    }

    const common::Nickname& GetNickname() const { return nickname_; }
    int GetElo() const { return elo_; }
    void SetElo(const int elo) { elo_ = elo; }

  private:
    const common::Nickname nickname_;
    int elo_;
};

#endif  // FOOSBALL_TRACKER_APP_MODEL_INCLUDE_PRIVATE_PLAYER_HPP_
