#include "players-detector.hpp"

void PlayersDetector::DetectBlueTeam(const cv::Mat &frame) {
  cv::Mat hsv, mask;

  cv::Scalar lower_blue(100, 150, 50);
  cv::Scalar upper_blue(140, 255, 255);

  cv::Mat kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(5, 5));

  cv::cvtColor(frame, hsv, cv::COLOR_BGR2HSV);

  cv::inRange(hsv, lower_blue, upper_blue, mask);

  cv::morphologyEx(mask, mask, cv::MORPH_OPEN, kernel);

//   std::vector<std::vector<cv::Point>> contours;
  std::vector<cv::Vec4i> hierarchy;
  cv::findContours(mask, players_.contours_blue_, hierarchy, cv::RETR_EXTERNAL,
                   cv::CHAIN_APPROX_SIMPLE);
}

void PlayersDetector::DetectRedTeam(const cv::Mat &frame) {
  cv::Mat hsv, mask1, mask2, mask;

  cv::Scalar lower_red1(0, 120, 70), upper_red1(10, 255, 255);
  cv::Scalar lower_red2(170, 120, 70), upper_red2(180, 255, 255);

  cv::Mat kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(5, 5));

  cv::cvtColor(frame, hsv, cv::COLOR_BGR2HSV);

  cv::inRange(hsv, lower_red1, upper_red1, mask1);
  cv::inRange(hsv, lower_red2, upper_red2, mask2);

  cv::bitwise_or(mask1, mask2, mask);

  cv::morphologyEx(mask, mask, cv::MORPH_OPEN, kernel);

//   std::vector<std::vector<cv::Point>> contours;
  std::vector<cv::Vec4i> hierarchy;
  cv::findContours(mask, players_.contours_red_, hierarchy, cv::RETR_EXTERNAL,
                   cv::CHAIN_APPROX_SIMPLE);
}

void PlayersDetector::DetectPlayers(const cv::Mat& frame)
{
    DetectRedTeam(frame);
    DetectBlueTeam(frame);
}

void PlayersDetector::DisplayPlayers(const cv::Mat& frame)
{
  for (const auto &contour : players_.contours_red_)
  {
    double area = cv::contourArea(contour);
    if (area > 500)
    {
      cv::Rect bounding_box = cv::boundingRect(contour);

      cv::rectangle(frame, bounding_box, cv::Scalar(0, 0, 255), 2);

      cv::putText(frame, "Red Player",
                  cv::Point(bounding_box.x, bounding_box.y - 10),
                  cv::FONT_HERSHEY_SIMPLEX, 0.6, cv::Scalar(0, 0, 255), 2);
    }
  }

  for (const auto &contour : players_.contours_blue_) {
    double area = cv::contourArea(contour);

    if (area > 500) {
      cv::Rect bounding_box = cv::boundingRect(contour);

      cv::rectangle(frame, bounding_box, cv::Scalar(0, 255, 0), 2);

      cv::putText(frame, "Blue Player",
                  cv::Point(bounding_box.x, bounding_box.y - 10),
                  cv::FONT_HERSHEY_SIMPLEX, 0.6, cv::Scalar(0, 255, 0), 2);
    }
  }

  cv::imshow("Foosball Tracking output", frame);
}