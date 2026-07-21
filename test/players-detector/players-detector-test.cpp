#include "players-detector.hpp"

#include <gtest/gtest.h>

TEST(PlayersDetector, CorrectPlayersAmount)
// int main()
{
    std::string image_path = std::string(TEST_SOURCE_DIR) + "/test-files/ball_unobscured.jpg";
    std::cout << image_path << std::endl;
    cv::Mat frame = cv::imread(image_path, cv::IMREAD_COLOR);
    if (frame.empty())
    {
        std::cerr << "Error: Could not open or find the image." << std::endl;
    }

    PlayersDetector players_detector{};

    players_detector.DetectPlayers(frame);
    players_detector.DisplayPlayers(frame);

    size_t team_blue_count{players_detector.players_.rectangles_blue_.size()};
    size_t team_red_count{players_detector.players_.rectangles_red_.size()};

    size_t constexpr actual_team_count = 11;
    EXPECT_EQ(team_blue_count, actual_team_count);
    EXPECT_EQ(team_red_count, actual_team_count);

    cv::waitKey(0);
    /*
    std::string video_path = std::string(TEST_SOURCE_DIR) + "/test-files/test_video.mp4";
    cv::VideoCapture cap(video_path);

    if (!cap.isOpened())
    {
        std::cerr << "Error: Could not open video source." << std::endl;
        return -1;
    }

    while (true)
    {
        cap >> frame;

        if (frame.empty())
        {
            std::cout << "End of video stream." << std::endl;
            break;
        }

        players_detector.DetectPlayers(frame);
        players_detector.DisplayPlayers(frame);
        players_detector.players_.rectangles_blue_.clear();
        players_detector.players_.rectangles_red_.clear();
        players_detector.players_.contours_blue_.clear();
        players_detector.players_.contours_red_.clear();

        char key = (char)cv::waitKey(33);
        if (key == 'q' || key == 27)
        {
            break;
        }
    }

    cap.release();
    cv::destroyAllWindows();
    return 0;
*/
}
