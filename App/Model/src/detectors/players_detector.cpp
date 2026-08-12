#include "players_detector.hpp"

#include <limits>
#include <opencv2/core/mat.hpp>
#include <opencv2/core/mat.inl.hpp>
#include <opencv2/core/matx.hpp>
#include <opencv2/core/traits.hpp>
#include <opencv2/core/types.hpp>
#include <opencv2/imgproc.hpp>
#include <ranges>
#include <tuple>
#include <vector>

#include "detector_types.hpp"
#include "mask_utils.hpp"

const Players& PlayersDetector::GetPlayers() const { return players_; }

template <typename ColorRange>
void PlayersDetector::DetectTeam(const cv::Mat& frame,
                                 std::vector<Contour>& contours,
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

    SeperateIntoOffenseAndDefense(size);
}

void PlayersDetector::SeperateIntoOffenseAndDefense(const cv::Size& size)
{
    constexpr int64_t number_of_player_rows = 8;
    const int64_t frame_width = size.width;
    const int64_t row_width = frame_width / number_of_player_rows;
    auto get_row_index = [row_width](int64_t x) { return x / row_width; };

    for (const auto& [rectangle_blue, rectangle_red] :
         std::views::zip(players_.rectangles_blue_, players_.rectangles_red_))
    {
        switch (get_row_index(rectangle_red.x))
        {
            case 0:
            case 1:
                players_.rectangles_red_defense_.push_back(rectangle_red);
                break;
            case 3:
            case 5:
                players_.rectangles_red_offense_.push_back(rectangle_red);
                break;
            default:
                break;
        }

        switch (get_row_index(rectangle_blue.x))
        {
            case 2:
            case 4:
                players_.rectangles_blue_offense_.push_back(rectangle_blue);
                break;
            case 6:
            case 7:
                players_.rectangles_blue_defense_.push_back(rectangle_blue);
                break;
            default:
                break;
        }
    }
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
        cv::rectangle(
          frame, rectangle, detector_types::kPlayersDrawBlueColor, detector_types::kDrawThickness);
    }
}