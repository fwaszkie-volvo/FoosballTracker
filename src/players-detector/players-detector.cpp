#include "players-detector.hpp"

void PlayersDetector::DetectBlueTeam(const cv::Mat &frame) {
  cv::Mat hsv, mask;

  cv::Scalar lower_blue(100, 150, 50);
  cv::Scalar upper_blue(140, 255, 255);

  cv::Mat kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(5, 5));

  cv::cvtColor(frame, hsv, cv::COLOR_BGR2HSV);

  cv::inRange(hsv, lower_blue, upper_blue, mask);

  cv::morphologyEx(mask, mask, cv::MORPH_OPEN, kernel);

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

  std::vector<cv::Vec4i> hierarchy;
  cv::findContours(mask, players_.contours_red_, hierarchy, cv::RETR_EXTERNAL,
                   cv::CHAIN_APPROX_SIMPLE);


}

void RemoveFalsePlayers()
{
  int64_t upper_right_x{-1};
  int64_t upper_right_y{-1};

  int64_t lower_right_x{-1};
  int64_t lower_right_y{-1};

  int64_t upper_left_x{-1};
  int64_t upper_left_y{-1};

  int64_t lower_left_x{-1};
  int64_t lower_left_y{-1};

  std::vector<std::vector<cv::Point>> false_players;

  for (const auto &contour : players_.contours_blue_)
  {
      cv::Rect bounding_box = cv::boundingRect(contour);
      if(bounding_box.x > upper_right_x)


   }
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

  int64_t blue_player_number = 0;
  std::string player_name = "Blue Player ";
  for (const auto &contour : players_.contours_blue_) {
    double area = cv::contourArea(contour);

    if (area > 500) {
      cv::Rect bounding_box = cv::boundingRect(contour);
      std::string coord_box = "(" + std::to_string(bounding_box.x) + ", " + std::to_string(bounding_box.y) + ")";

      cv::rectangle(frame, bounding_box, cv::Scalar(255, 0, 0), 2);

      blue_player_number++;
      player_name += std::to_string(blue_player_number);
      cv::putText(frame, player_name,
                  cv::Point(bounding_box.x - 20, bounding_box.y - 10),
                  cv::FONT_HERSHEY_SIMPLEX, 0.6, cv::Scalar(255, 0, 0), 2);
      cv::putText(frame, coord_box,
                  cv::Point(bounding_box.x - 100, bounding_box.y - 100),
                  cv::FONT_HERSHEY_SIMPLEX, 0.6, cv::Scalar(255, 0, 0), 2);
      player_name.resize(player_name.size() - std::to_string(blue_player_number).size());
    }
  }

  cv::imshow("Foosball Tracking output", frame);
}