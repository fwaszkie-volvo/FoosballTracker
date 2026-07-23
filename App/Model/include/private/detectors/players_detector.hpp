#ifndef PLAYERS_DETECTOR_HPP_
#define PLAYERS_DETECTOR_HPP_

#include <opencv2/opencv.hpp>

struct Players
{
    std::vector<std::vector<cv::Point>> contours_blue_;
    std::vector<std::vector<cv::Point>> contours_red_;

    std::vector<cv::Rect> rectangles_blue_;
    std::vector<cv::Rect> rectangles_red_;
};

class PlayersDetector
{
  public:
    void DetectPlayers(const cv::Mat& frame);

    // Debug only, to be removed in final version
    void DisplayPlayers(const cv::Mat& frame);

    Players players_{};

  private:
    static int64_t DistanceToCorner(const cv::Rect& rectangle, const cv::Point& corner);
    template <typename ColorRange>
    void DetectTeamImpl(const cv::Mat& frame,
                        std::vector<std::vector<cv::Point>>& contours,
                        std::vector<cv::Rect>& rectangles);
    static void RemoveInvalidPlayers(std::vector<cv::Rect>& rectangles,
                                     const cv::Point& first_corner,
                                     const cv::Point& second_corner);
};

#endif /* PLAYERS_DETECTOR_HPP_ */
