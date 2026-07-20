#include "players-detector.hpp"

// TEST(PlayersDetector, CorrectPlayersAmount)
int main()
{
  std::string image_path = std::string(TEST_SOURCE_DIR) + "/test-files/ball_unobscured.jpg";
  std::cout << image_path << std::endl;
  cv::Mat frame = cv::imread(image_path, cv::IMREAD_COLOR);
    if (frame.empty()) {
        std::cerr << "Error: Could not open or find the image." << std::endl;
        return -1;
    }

  PlayersDetector players_detector{};

  players_detector.DetectPlayers(frame);
  players_detector.DisplayPlayers(frame);

  int64_t player_count = 0;
  for (const auto &contour : players_detector.players_.contours_red_)
  {
    double area = cv::contourArea(contour);
    player_count++;
    std::cout << "   " << player_count << std::endl;
    for(const auto& points : contour)
    {
        std::cout << points << std::endl;
    }
  }
//   std::cout << player_count << std::endl;
  cv::waitKey(0);
}
