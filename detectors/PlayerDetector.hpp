#ifndef PLAYER_DETECTOR_HPP_
#define PLAYER_DETECTOR_HPP_

#include <string>

#include <opencv2/opencv.hpp>

class PlayerDetector
{
public:
    PlayerDetector(const cv::Scalar &lower,
                   const cv::Scalar &upper,
                   const cv::Scalar &draw_color,
                   const std::string &label);

    PlayerDetector(const cv::Scalar &lower1,
                   const cv::Scalar &upper1,
                   const cv::Scalar &lower2,
                   const cv::Scalar &upper2,
                   const cv::Scalar &draw_color,
                   const std::string &label);

    cv::Mat Detect(cv::Mat &frame) const;

private:
    void DrawDetections(cv::Mat &frame, const cv::Mat &mask) const;

    bool dual_range_;
    cv::Scalar lower1_;
    cv::Scalar upper1_;
    cv::Scalar lower2_;
    cv::Scalar upper2_;
    cv::Scalar draw_color_;
    std::string label_;
};

cv::Mat detect_blue(cv::Mat &frame);
cv::Mat detect_red(cv::Mat &frame);

#endif /* PLAYER_DETECTOR_HPP_ */
