#ifndef FOOSBALL_TRACKER_APP_MODEL_INCLUDE_PRIVATE_PLAYER_HPP_
#define FOOSBALL_TRACKER_APP_MODEL_INCLUDE_PRIVATE_PLAYER_HPP_

#include <string>
#include <utility>

static constexpr int kDefaultElo{1000};

class Player
{
  public:
    Player(std::string nickname, const int elo = kDefaultElo)
        : nickname_(std::move(nickname)), elo_(elo)
    {
    }

    const std::string& GetNickname() const { return nickname_; }
    int GetElo() const { return elo_; }
    void SetElo(const int elo) { elo_ = elo; }

  private:
    const std::string nickname_;
    int elo_;
};

#endif  // FOOSBALL_TRACKER_APP_MODEL_INCLUDE_PRIVATE_PLAYER_HPP_
