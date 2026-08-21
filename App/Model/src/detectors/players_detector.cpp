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

namespace player_detector
{
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
                                           const cv::Point& upper_corner,
                                           const cv::Point& lower_corner,
                                           const cv::Mat& frame)
{
    cv::Rect false_player_1{};
    cv::Rect false_player_2{};
    int64_t distance_upper{std::numeric_limits<int64_t>::max()};
    int64_t distance_lower{std::numeric_limits<int64_t>::max()};
    constexpr int64_t number_of_player_rows{8};
    const int64_t frame_width{frame.size().width};
    const int64_t first_row{frame_width / number_of_player_rows};
    const int64_t seventh_row{7 * first_row};
    int64_t keeper_row{};
    std::function<bool(int, int)> compare;

    if (upper_corner == cv::Point{0, 0})
    {
        keeper_row = first_row;
        compare    = std::less<int>();
    }
    else
    {
        keeper_row = seventh_row;
        compare    = std::greater<int>();
    }

    for (const auto& rectangle : rectangles)
    {
        const int64_t current_distance_upper = DistanceToCorner(rectangle, upper_corner);
        const int64_t current_distance_lower = DistanceToCorner(rectangle, lower_corner);

        if (current_distance_upper < distance_upper && compare(keeper_row, rectangle.x))
        {
            distance_upper = current_distance_upper;
            false_player_1 = rectangle;
        }
        if (current_distance_lower < distance_lower && compare(keeper_row, rectangle.x))
        {
            distance_lower = current_distance_lower;
            false_player_2 = rectangle;
        }
    }
    cv::rectangle(frame,
                  false_player_1,
                  detector_types::kPlayersDrawDebugBlackColor,
                  detector_types::kDrawThickness);
    cv::rectangle(frame,
                  false_player_2,
                  detector_types::kPlayersDrawDebugBlackColor,
                  detector_types::kDrawThickness);
    std::erase(rectangles, false_player_1);
    std::erase(rectangles, false_player_2);
}

void PlayersDetector::Detect(const cv::Mat& frame)
{
    DetectTeam<detector_types::RedColorRange>(frame, contours_red_, rectangles_red_);
    DetectTeam<detector_types::BlueColorRange>(frame, contours_blue_, rectangles_blue_);

    const cv::Size size = frame.size();
    RemoveInvalidPlayers(
      rectangles_blue_, cv::Point{size.width, 0}, cv::Point{size.width, size.height}, frame);
    RemoveInvalidPlayers(rectangles_red_, cv::Point{0, 0}, cv::Point{0, size.height}, frame);

    constexpr uint32_t red_mask  = 0b00'00'10'00'10'00'01'01;
    constexpr uint32_t blue_mask = 0b01'01'00'10'00'10'00'00;
    SeperateTeamIntoOffenseAndDefense(players_.red_team_, rectangles_red_, red_mask, size.width);
    SeperateTeamIntoOffenseAndDefense(players_.blue_team_, rectangles_blue_, blue_mask, size.width);
}

void PlayersDetector::SeperateTeamIntoOffenseAndDefense(Team& team,
                                                        const std::vector<cv::Rect>& rectangles,
                                                        const uint32_t mask,
                                                        const int64_t frame_width)
{
    // We make a mask of each teams position of offense and deffense,
    // below are the indicies of rows in the mask
    // each ?? can be one of the roles below
    //            7  6  5  4  3  2  1  0
    // mask = 0b ?? ?? ?? ?? ?? ?? ?? ??

    constexpr int64_t number_of_player_rows = 8;
    constexpr uint32_t role_mask_width      = 2;
    constexpr uint32_t role_none            = 0b00;
    constexpr uint32_t role_defense         = 0b01;
    constexpr uint32_t role_offense         = 0b10;

    const int64_t row_width = frame_width / number_of_player_rows;

    for (const auto& rectangle : rectangles)
    {
        int64_t row       = rectangle.x / row_width;
        int64_t row_shift = row * role_mask_width;

        uint32_t role = mask >> row_shift;

        if (role & role_defense)
        {
            team.defense_.push_back(rectangle);
        }

        else if (role & role_offense)
        {
            team.offense_.push_back(rectangle);
        }
    }
}

void PlayersDetector::Draw(const cv::Mat& frame)
{
    for (const auto& [rectangle_offense, rectangle_deffense] :
         std::views::zip(players_.red_team_.offense_, players_.red_team_.defense_))
    {
        cv::rectangle(frame,
                      rectangle_offense,
                      detector_types::kPlayersDrawRedColor,
                      detector_types::kDrawThickness);
        cv::rectangle(frame,
                      rectangle_deffense,
                      detector_types::kPlayersDrawRedColor,
                      detector_types::kDrawThickness);
    }

    for (const auto& [rectangle_offense, rectangle_deffense] :
         std::views::zip(players_.blue_team_.offense_, players_.blue_team_.defense_))
    {
        cv::rectangle(frame,
                      rectangle_offense,
                      detector_types::kPlayersDrawBlueColor,
                      detector_types::kDrawThickness);
        cv::rectangle(frame,
                      rectangle_deffense,
                      detector_types::kPlayersDrawBlueColor,
                      detector_types::kDrawThickness);
    }
}
}  // namespace player_detector