#ifndef FOOSBALL_TRACKER_APP_MODEL_INCLUDE_PUBLIC_MODEL_MAIN_HPP_
#define FOOSBALL_TRACKER_APP_MODEL_INCLUDE_PUBLIC_MODEL_MAIN_HPP_

#include <optional>

#include <opencv2/core/mat.hpp>

#include "generator_types.hpp"

class ModelMain
{
  public:
    std::optional<cv::Mat> Calculate();
    std::optional<generator::TeamDrawResult> GenerateTeams(const generator::Players& players) const;
};

#endif  // FOOSBALL_TRACKER_APP_MODEL_INCLUDE_PUBLIC_MODEL_MAIN_HPP_
