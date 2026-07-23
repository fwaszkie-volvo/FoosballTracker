#include "players_detector.hpp"

#include <algorithm>
#include <cstdint>
#include <limits>
#include <opencv2/core/mat.hpp>
#include <opencv2/core/mat.inl.hpp>
#include <opencv2/core/matx.hpp>
#include <opencv2/core/types.hpp>
#include <opencv2/imgproc.hpp>
#include <string>
#include <vector>

#include "detector_types.hpp"
#include "mask_utils.hpp"

const Players& PlayersDetector::GetPlayers() const { return players_; }

template <typename ColorRange>
void PlayersDetector::DetectTeam(const cv::Mat& frame,
                                 std::vector<std::vector<cv::Point>>& contours,
                                 std::vector<cv::Rect>& rectangles)
{
    cv::Mat mask{ColorRange::CreateMask(frame)};
    cv::Mat kernel{mask_utils::create_kernel(detector_types::kPlayersKernelSize, cv::MORPH_RECT)};
    cv::morphologyEx(mask, mask, cv::MORPH_OPEN, kernel);

    std::vector<cv::Vec4i> hierarchy;
    cv::findContours(mask, contours, hierarchy, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

    for (const auto& contour : contours)
    {
        const double area{cv::contourArea(contour)};
        if (area > detector_types::kPlayersMinArea)
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

void PlayersDetector::Detect(const cv::Mat& frame)
{
    DetectTeam<detector_types::RedColorRange>(
      frame, players_.contours_red_, players_.rectangles_red_);
    DetectTeam<detector_types::BlueColorRange>(
      frame, players_.contours_blue_, players_.rectangles_blue_);
    const cv::Size size = frame.size();
    RemoveInvalidPlayers(
      players_.rectangles_blue_, cv::Point{size.width, 0}, cv::Point{size.width, size.height});
    RemoveInvalidPlayers(players_.rectangles_red_, cv::Point{0, 0}, cv::Point{0, size.height});
}

void PlayersDetector::Draw(const cv::Mat& frame)
{
    for (const auto& rectangle : players_.rectangles_red_)
    {
        cv::rectangle(
          frame, rectangle, detector_types::kPlayersDrawRedColor, detector_types::kDrawThickness);
    }

    for (const auto& rectangle : players_.rectangles_blue_)
    {
        std::string coord_box =
          "(" + std::to_string(rectangle.x) + ", " + std::to_string(rectangle.y) + ")";

        cv::rectangle(
          frame, rectangle, detector_types::kPlayersDrawBlueColor, detector_types::kDrawThickness);
    }
}