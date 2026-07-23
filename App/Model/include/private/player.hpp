#ifndef FOOSBALL_TRACKER_APP_MODEL_INCLUDE_PRIVATE_PLAYER_HPP_
#define FOOSBALL_TRACKER_APP_MODEL_INCLUDE_PRIVATE_PLAYER_HPP_

#include <string>
#include <utility>

class Player
{
  public:
    explicit Player(std::string nickname) : nickname_(std::move(nickname)) {}

    const std::string& GetNickname() const { return nickname_; }

  private:
    std::string nickname_;
};

#endif  // FOOSBALL_TRACKER_APP_MODEL_INCLUDE_PRIVATE_PLAYER_HPP_
