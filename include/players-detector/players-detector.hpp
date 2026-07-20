#ifndef TRACKPLAYERS_HPP_
#define TRACKPLAYERS_HPP_

#include <opencv2/opencv.hpp>

struct Players
{
    std::vector<std::vector<cv::Point>> contours_blue_;
    std::vector<std::vector<cv::Point>> contours_red_;
};

class PlayersDetector
{
public:
    void DetectPlayers(const cv::Mat& frame);
    void DisplayPlayers(const cv::Mat& frame);

private:
    void DetectRedTeam(const cv::Mat &frame);
    void DetectBlueTeam(const cv::Mat &frame);

    Players players_;
};

#endif