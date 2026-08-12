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

    // We make a mask of each teams position of offense and deffense, below are the indicies of rows in the mask
    //     7  6  5  4  3  2  1  0
    // 0b ?? ?? ?? ?? ?? ?? ?? ??,
    constexpr uint32_t role_mask_width = 2;
    constexpr uint32_t role_none = 0b00;
    constexpr uint32_t role_defense = 0b01;
    constexpr uint32_t role_offense = 0b10;
    constexpr uint32_t red_mask = 0b0000100010000101;
    constexpr uint32_t blue_mask = 0b0101001000100000;

    for (const auto& rectangle_red : players_.rectangles_red_)
    {
        int64_t row = rectangle_red.x / row_width;
        int64_t row_shift = row * role_mask_width;

        uint32_t role = red_mask >> row_shift;

        if (role & role_defense)
        {
            players_.rectangles_red_defense_.push_back(rectangle_red);
        }

        else if (role & role_offense)
        {
            players_.rectangles_red_offense_.push_back(rectangle_red);
        }
    }

    for (const auto& rectangle_blue : players_.rectangles_blue_)
    {
        int64_t row = rectangle_blue.x / row_width;
        int64_t row_shift = row * role_mask_width;

        uint32_t role = blue_mask >> row_shift;

        if (role & role_defense)
        {
            players_.rectangles_blue_defense_.push_back(rectangle_blue);
        }

        else if (role & role_offense)
        {
            players_.rectangles_blue_offense_.push_back(rectangle_blue);
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