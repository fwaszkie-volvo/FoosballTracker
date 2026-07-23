#ifndef FOOSBALL_TRACKER_APP_MODEL_INCLUDE_PUBLIC_MODEL_MAIN_HPP_
#define FOOSBALL_TRACKER_APP_MODEL_INCLUDE_PUBLIC_MODEL_MAIN_HPP_

#include <optional>

#include "generator_types.hpp"

class ModelMain
{
  public:
    void Calculate();
    std::optional<generator::TeamDraw> GenerateTeams(const generator::Players& nicknames) const;
};

#endif  // FOOSBALL_TRACKER_APP_MODEL_INCLUDE_PUBLIC_MODEL_MAIN_HPP_
