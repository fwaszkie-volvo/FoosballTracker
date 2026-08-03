#ifndef FOOSBALL_TRACKER_APP_MODEL_INCLUDE_PRIVATE_DETECTORS_PLAYERS_DETECTOR_HPP_
#define FOOSBALL_TRACKER_APP_MODEL_INCLUDE_PRIVATE_DETECTORS_PLAYERS_DETECTOR_HPP_

#include <stdint.h>

#include <opencv2/core/mat.hpp>
#include <opencv2/core/types.hpp>
#include <vector>

#include "detector_types.hpp"

constexpr int64_t defense_count = 3;
constexpr int64_t offense_count = 8;

struct Players
{
    std::vector<Contour> contours_blue_;
    std::vector<Contour> contours_red_;

    std::vector<cv::Rect> rectangles_blue_;
    std::vector<cv::Rect> rectangles_red_;

    std::vector<cv::Rect> rectangles_blue_defense_;
    std::vector<cv::Rect> rectangles_blue_offense_;
    std::vector<cv::Rect> rectangles_red_defense_;
    std::vector<cv::Rect> rectangles_red_offense_;
};

class PlayersDetector
{
  public:
    void Detect(const cv::Mat& frame);
    void Draw(const cv::Mat& frame);
    const Players& GetPlayers() const;

  private:
    Players players_{};

    static int64_t DistanceToCorner(const cv::Rect& rectangle, const cv::Point& corner);
    template <typename ColorRange>
    void DetectTeam(const cv::Mat& frame,
                    std::vector<Contour>& contours,
                    std::vector<cv::Rect>& rectangles);
    static void RemoveInvalidPlayers(std::vector<cv::Rect>& rectangles,
                                     const cv::Point& first_corner,
                                     const cv::Point& second_corner);

    void SeperateIntoOffenseAndDefense(const cv::Mat& frame);
};

#endif  // FOOSBALL_TRACKER_APP_MODEL_INCLUDE_PRIVATE_DETECTORS_PLAYERS_DETECTOR_HPP_
