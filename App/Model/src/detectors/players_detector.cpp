#include "players_detector.hpp"

void PlayersDetector::DetectBlueTeam(const cv::Mat& frame)
{
    cv::Mat hsv, mask;

    cv::Scalar lower_blue(100, 150, 50);
    cv::Scalar upper_blue(140, 255, 255);

    cv::Mat kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(5, 5));

    cv::cvtColor(frame, hsv, cv::COLOR_BGR2HSV);

    cv::inRange(hsv, lower_blue, upper_blue, mask);

    cv::morphologyEx(mask, mask, cv::MORPH_OPEN, kernel);

    std::vector<cv::Vec4i> hierarchy;
    cv::findContours(
      mask, players_.contours_blue_, hierarchy, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

    for (const auto& contour : players_.contours_blue_)
    {
        double area = cv::contourArea(contour);
        if (area > 500)
        {
            cv::Rect bounding_box = cv::boundingRect(contour);
            players_.rectangles_blue_.push_back(bounding_box);
        }
    }
}

void PlayersDetector::DetectRedTeam(const cv::Mat& frame)
{
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
    cv::findContours(
      mask, players_.contours_red_, hierarchy, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

    for (const auto& contour : players_.contours_red_)
    {
        double area = cv::contourArea(contour);
        if (area > 500)
        {
            cv::Rect bounding_box = cv::boundingRect(contour);
            players_.rectangles_red_.push_back(bounding_box);
        }
    }
}

void PlayersDetector::RemoveFalseBluePlayers(const cv::Size size)
{
    cv::Rect false_player_blue_1{};
    cv::Rect false_player_blue_2{};
    int64_t distance_1{std::numeric_limits<int>::max()};
    int64_t distance_2{std::numeric_limits<int>::max()};
    int64_t current_distance_1{-1};
    int64_t current_distance_2{-1};

    for (const auto& rectangle : players_.rectangles_blue_)
    {
        current_distance_1 =
          (rectangle.x - size.width) * (rectangle.x - size.width) + (rectangle.y) * (rectangle.y);

        current_distance_2 = (rectangle.x - size.width) * (rectangle.x - size.width) +
                             (rectangle.y - size.height) * (rectangle.y - size.height);

        if (current_distance_1 < distance_1)
        {
            distance_1 = current_distance_1;
            false_player_blue_1 = rectangle;
        }
        if (current_distance_2 < distance_2)
        {
            distance_2 = current_distance_2;
            false_player_blue_2 = rectangle;
        }
    }

    std::erase(players_.rectangles_blue_, false_player_blue_1);
    std::erase(players_.rectangles_blue_, false_player_blue_2);
}

void PlayersDetector::RemoveFalseRedPlayers(const cv::Size size)
{
    cv::Rect false_player_red_1{};
    cv::Rect false_player_red_2{};
    int64_t distance_1{std::numeric_limits<int>::max()};
    int64_t distance_2{std::numeric_limits<int>::max()};
    int64_t current_distance_1{-1};
    int64_t current_distance_2{-1};

    for (const auto& rectangle : players_.rectangles_red_)
    {
        current_distance_1 = (rectangle.x) * (rectangle.x) + (rectangle.y) * (rectangle.y);

        current_distance_2 =
          (rectangle.x) * (rectangle.x) + (rectangle.y - size.height) * (rectangle.y - size.height);
        if (current_distance_1 < distance_1)
        {
            distance_1 = current_distance_1;
            false_player_red_1 = rectangle;
        }
        if (current_distance_2 < distance_2)
        {
            distance_2 = current_distance_2;
            false_player_red_2 = rectangle;
        }
    }

    std::erase(players_.rectangles_red_, false_player_red_1);
    std::erase(players_.rectangles_red_, false_player_red_2);
}

void PlayersDetector::DetectPlayers(const cv::Mat& frame)
{
    DetectRedTeam(frame);
    DetectBlueTeam(frame);
    RemoveFalseRedPlayers(frame.size());
    RemoveFalseBluePlayers(frame.size());
}

void PlayersDetector::DisplayPlayers(const cv::Mat& frame)
{
    cv::Scalar red = cv::Scalar(0, 0, 255);
    cv::Scalar blue = cv::Scalar(255, 0, 0);

    for (const auto& rectangle : players_.rectangles_red_)
    {
        cv::rectangle(frame, rectangle, red, 2);
    }

    for (const auto& rectangle : players_.rectangles_blue_)
    {
        std::string coord_box =
          "(" + std::to_string(rectangle.x) + ", " + std::to_string(rectangle.y) + ")";

        cv::rectangle(frame, rectangle, blue, 2);
    }
}