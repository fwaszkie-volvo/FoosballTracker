#include "players_detector.hpp"

struct BlueColorRange
{
    static cv::Mat CreateMask(const cv::Mat& hsv)
    {
        cv::Mat mask;
        cv::inRange(hsv, cv::Scalar(100, 150, 50), cv::Scalar(140, 255, 255), mask);
        return mask;
    }
};

struct RedColorRange
{
    static cv::Mat CreateMask(const cv::Mat& hsv)
    {
        cv::Mat mask1, mask2;
        cv::inRange(hsv, cv::Scalar(0, 120, 70), cv::Scalar(10, 255, 255), mask1);
        cv::inRange(hsv, cv::Scalar(170, 120, 70), cv::Scalar(180, 255, 255), mask2);
        cv::Mat mask;
        cv::bitwise_or(mask1, mask2, mask);
        return mask;
    }
};

template <typename ColorRange>
void PlayersDetector::DetectTeamImpl(const cv::Mat& frame,
                                     std::vector<std::vector<cv::Point>>& contours,
                                     std::vector<cv::Rect>& rectangles)
{
    cv::Mat hsv;
    cv::cvtColor(frame, hsv, cv::COLOR_BGR2HSV);

    cv::Mat mask = ColorRange::CreateMask(hsv);

    cv::Mat kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(5, 5));
    cv::morphologyEx(mask, mask, cv::MORPH_OPEN, kernel);

    std::vector<cv::Vec4i> hierarchy;
    cv::findContours(mask, contours, hierarchy, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

    for (const auto& contour : contours)
    {
        double area = cv::contourArea(contour);
        if (area > 500)
        {
            rectangles.push_back(cv::boundingRect(contour));
        }
    }
}

int64_t PlayersDetector::DistanceToCorner(const cv::Rect& rectangle, const cv::Point& corner)
{
    const int64_t delta_x = rectangle.x - corner.x;
    const int64_t delta_y = rectangle.y - corner.y;
    return delta_x * delta_x + delta_y * delta_y;
}

void PlayersDetector::RemoveInvalidPlayers(std::vector<cv::Rect>& rectangles,
                                           const cv::Point& first_corner,
                                           const cv::Point& second_corner)
{
    cv::Rect false_player_1{};
    cv::Rect false_player_2{};
    int64_t distance_1{std::numeric_limits<int64_t>::max()};
    int64_t distance_2{std::numeric_limits<int64_t>::max()};

    for (const auto& rectangle : rectangles)
    {
        const int64_t current_distance_1 = DistanceToCorner(rectangle, first_corner);
        const int64_t current_distance_2 = DistanceToCorner(rectangle, second_corner);

        if (current_distance_1 < distance_1)
        {
            distance_1 = current_distance_1;
            false_player_1 = rectangle;
        }
        if (current_distance_2 < distance_2)
        {
            distance_2 = current_distance_2;
            false_player_2 = rectangle;
        }
    }

    std::erase(rectangles, false_player_1);
    std::erase(rectangles, false_player_2);
}

void PlayersDetector::DetectPlayers(const cv::Mat& frame)
{
    DetectTeamImpl<RedColorRange>(frame, players_.contours_red_, players_.rectangles_red_);
    DetectTeamImpl<BlueColorRange>(frame, players_.contours_blue_, players_.rectangles_blue_);
    const cv::Size size = frame.size();
    RemoveInvalidPlayers(
      players_.rectangles_blue_, cv::Point{size.width, 0}, cv::Point{size.width, size.height});
    RemoveInvalidPlayers(players_.rectangles_red_, cv::Point{0, 0}, cv::Point{0, size.height});
}

void PlayersDetector::DisplayPlayers(const cv::Mat& frame)
{
    const cv::Scalar red{cv::Scalar(0, 0, 255)};
    const cv::Scalar blue{cv::Scalar(255, 0, 0)};

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