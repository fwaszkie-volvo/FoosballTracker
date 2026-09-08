#ifndef FOOSBALL_TRACKER_APP_MODEL_INCLUDE_PRIVATE_DETECTORS_PLAYERS_DETECTOR_HPP_
#define FOOSBALL_TRACKER_APP_MODEL_INCLUDE_PRIVATE_DETECTORS_PLAYERS_DETECTOR_HPP_

#include <stdint.h>

#include <opencv2/core/mat.hpp>
#include <opencv2/core/types.hpp>
#include <vector>

#include "detector.hpp"
#include "detector_types.hpp"
#include "model_types.hpp"

class PlayersDetector : public IDetector
{
  public:
    void Detect(const cv::Mat& frame) override;
    void Draw(cv::Mat& frame) const override;
    const model::PlayersPositions& GetPlayers() const;

  private:
    model::PlayersPositions players_{};

    std::vector<Contour> contours_blue_;
    std::vector<Contour> contours_red_;

    std::vector<cv::Rect> rectangles_blue_;
    std::vector<cv::Rect> rectangles_red_;

    static int64_t DistanceToCorner(const cv::Rect& rectangle, const cv::Point& corner);
    template <typename ColorRange>
    void DetectTeam(const cv::Mat& frame,
                    std::vector<Contour>& contours,
                    std::vector<cv::Rect>& rectangles);
    static void RemoveInvalidPlayers(std::vector<cv::Rect>& rectangles,
                                     const cv::Point& first_corner,
                                     const cv::Point& second_corner);
    void SeperateTeamIntoOffenseAndDefense(model::PlayerPositions& team,
                                           const std::vector<cv::Rect>& rectangles,
                                           const uint32_t mask,
                                           const int64_t frame_width);
};
#endif  // FOOSBALL_TRACKER_APP_MODEL_INCLUDE_PRIVATE_DETECTORS_PLAYERS_DETECTOR_HPP_
