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
    cv::Mat hsv, mask;

    cv::Scalar lower_blue(100, 150, 50);
    cv::Scalar upper_blue(140, 255, 255);

    cv::Mat kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(5, 5));

    cv::cvtColor(frame, hsv, cv::COLOR_BGR2HSV);

    cv::inRange(hsv, lower_blue, upper_blue, mask);

    cv::morphologyEx(mask, mask, cv::MORPH_OPEN, kernel);

    std::vector<std::vector<cv::Point>> contours;
    std::vector<cv::Vec4i> hierarchy;
    cv::findContours(mask, contours, hierarchy, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

    for (const auto& contour : contours)
    {
        double area = cv::contourArea(contour);

        if (area > 500)
        {
            cv::Rect bounding_box = cv::boundingRect(contour);

            cv::rectangle(frame, bounding_box, cv::Scalar(0, 255, 0), 2);

            cv::putText(frame, "Blue Player", cv::Point(bounding_box.x, bounding_box.y - 10),
                        cv::FONT_HERSHEY_SIMPLEX, 0.6, cv::Scalar(0, 255, 0), 2);
        }
    }

    return frame;
}

cv::Mat detect_red(const cv::Mat& frame)
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

    std::vector<std::vector<cv::Point>> contours;
    std::vector<cv::Vec4i> hierarchy;
    cv::findContours(mask, contours, hierarchy, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

    for (const auto& contour : contours)
    {
        double area = cv::contourArea(contour);
        if (area > 500)
        {
            cv::Rect bounding_box = cv::boundingRect(contour);

            cv::rectangle(frame, bounding_box, cv::Scalar(0, 0, 255), 2);

            cv::putText(frame, "Red Player", cv::Point(bounding_box.x, bounding_box.y - 10),
                        cv::FONT_HERSHEY_SIMPLEX, 0.6, cv::Scalar(0, 0, 255), 2);
        }
    }

    return frame;
}

int main()
{
    cv::Mat frame = cv::imread("ball_unobscured.jpg", cv::IMREAD_COLOR);

    detect_blue(frame);
    detect_red(frame);

    cv::imshow("Foosball Tracking output", frame);

    cv::waitKey(0);

    return 0;
}


