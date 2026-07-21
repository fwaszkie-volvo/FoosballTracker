#ifndef TRACKPLAYERS_HPP_
#define TRACKPLAYERS_HPP_

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

    Players players_;

  private:
    void DetectRedTeam(const cv::Mat& frame);
    void DetectBlueTeam(const cv::Mat& frame);
    void RemoveFalseRedPlayers(const cv::Size size);
    void RemoveFalseBluePlayers(const cv::Size size);
};

#endif