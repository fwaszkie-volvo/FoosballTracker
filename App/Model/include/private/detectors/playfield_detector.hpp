#ifndef FOOSBALL_TRACKER_APP_MODEL_INCLUDE_PRIVATE_DETECTORS_PLAYFIELD_DETECTOR_HPP_
#define FOOSBALL_TRACKER_APP_MODEL_INCLUDE_PRIVATE_DETECTORS_PLAYFIELD_DETECTOR_HPP_

#include <opencv2/core/mat.hpp>
#include <opencv2/core/types.hpp>
#include <vector>

class PlayfieldDetector
{
  public:
    bool Detect(const cv::Mat &frame,
                std::vector<cv::Point> &playfield_polygon,
                cv::Mat &playfield_mask) const;

  private:
    bool ChooseLargestContour(const std::vector<std::vector<cv::Point>> &contours,
                              std::vector<cv::Point> &largest_contour) const;
    std::vector<cv::Point> ApproximatePolygon(const std::vector<cv::Point> &hull) const;
};

#endif  // FOOSBALL_TRACKER_APP_MODEL_INCLUDE_PRIVATE_DETECTORS_PLAYFIELD_DETECTOR_HPP_
