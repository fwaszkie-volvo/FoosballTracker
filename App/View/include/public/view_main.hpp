#ifndef FOOSBALL_TRACKER_APP_VIEW_INCLUDE_PUBLIC_VIEW_MAIN_HPP_
#define FOOSBALL_TRACKER_APP_VIEW_INCLUDE_PUBLIC_VIEW_MAIN_HPP_

#include <optional>

#include <opencv2/core/mat.hpp>

class ViewMain
{
  public:
    void Draw(const std::optional<cv::Mat>& frame);
};

#endif  // FOOSBALL_TRACKER_APP_VIEW_INCLUDE_PUBLIC_VIEW_MAIN_HPP_
