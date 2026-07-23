#ifndef FOOSBALL_TRACKER_APP_MODEL_INCLUDE_PUBLIC_MODEL_MAIN_HPP_
#define FOOSBALL_TRACKER_APP_MODEL_INCLUDE_PUBLIC_MODEL_MAIN_HPP_

#include <array>
#include <optional>
#include <string>
#include <vector>

static constexpr std::size_t kPlayersCount{4};

class ModelMain
{
  public:
    void Calculate();
    std::optional<TeamDraw> GenerateTeams(
      const std::array<std::string, kPlayersCount>& nicknames) const;
};

#endif  // FOOSBALL_TRACKER_APP_MODEL_INCLUDE_PUBLIC_MODEL_MAIN_HPP_
