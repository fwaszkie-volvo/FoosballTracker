#include "players-detector.hpp"
#include <opencv2/opencv.hpp>

/*
  Track:
Possesion - time between a teams first and last touch of a ball, once the ball
is touched by the opposite team it's in their possesion Shots - amount of balls
crossing the double defense Shots on target - amount of balls that would've went
in if not for double defense and goalkeeper

  Algorithm:
1. Capture video feed (stats don't have to be in real time, a slight delay is
okay since we are processing 240fps)
2. Detect ball
3. Detect if collision with player (if previous frame velocity switched signs
and collision occured around player)

*/

int main() {
  cv::Mat frame = cv::imread("ball_unobscured.jpg", cv::IMREAD_COLOR);

  PlayersDetector players_detector{};

  players_detector.DetectPlayers(frame);
  players_detector.DisplayPlayers(frame);

  cv::waitKey(0);

  return 0;
}
