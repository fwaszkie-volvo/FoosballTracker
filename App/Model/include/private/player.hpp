#ifndef FOOSBALL_TRACKER_APP_MODEL_INCLUDE_PRIVATE_PLAYER_HPP_
#define FOOSBALL_TRACKER_APP_MODEL_INCLUDE_PRIVATE_PLAYER_HPP_

#include <string>
#include <utility>
#include "model_types.hpp"

static constexpr int kDefaultElo{1000};
using Nickname       = std::string;

class Player
{
  public:
    Player(Nickname nickname, const int elo = kDefaultElo)
        : nickname_(std::move(nickname)), elo_(elo)
    {
    }

    const Nickname& GetNickname() const { return nickname_; }
    int GetElo() const { return elo_; }
    void SetElo(const int elo) { elo_ = elo; }

  private:
    const Nickname nickname_;
    int elo_;
};

#endif // FOOSBALL_TRACKER_APP_MODEL_INCLUDE_PRIVATE_PLAYER_HPP_
