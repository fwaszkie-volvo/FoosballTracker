#include <opencv2/opencv.hpp>

/*
  Track:
Possesion - time between a teams first and last touch of a ball, once the ball is touched by the opposite team it's in their possesion
Shots - amount of balls crossing the double defense
Shots on target - amount of balls that would've went in if not for double defense and goalkeeper

  Algorithm:
1. Capture video feed (stats don't have to be in real time, a slight delay is okay since we are processing 240fps)
2. Detect ball
3. Detect if collision with player (if previous frame velocity switched signs and collision occured around player)

*/

cv::Mat detect_blue(const cv::Mat &frame)
{
    cv::Mat hsv, b_mask, blue;

    cv::Scalar L_limit(98, 50, 50);
    cv::Scalar U_limit(139, 255, 255);

    cv::cvtColor(frame, hsv, cv::COLOR_BGR2HSV);

    cv::inRange(into_hsv, L_limit, U_limit, b_mask);

    cv::bitwise_and(frame, frame, blue, b_mask);

    return blue;
}

int main()
{
    cv::Mat frame = cv::imread("ball_unobscured.jpg", cv::IMREAD_COLOR);

    cv::Mat result = detect(frame);

    cv::imshow("Foosball Tracking Output", result);
    cv::waitKey(0);

    return 0;
}


