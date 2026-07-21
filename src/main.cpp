#include <opencv2/opencv.hpp>

#include "detector-config.hpp"
#include "detector.hpp"

int main()
{
    cv::Mat frame = cv::imread(detector_config::kInputImagePath, cv::IMREAD_COLOR);
    if (frame.empty())
    {
        std::cerr << "Failed to load input image: " << detector_config::kInputImagePath << "\n";
        return 1;
    }

    detect_ball(frame);

    cv::imwrite(detector_config::kOutputImagePath, frame);

    // Only display if running in interactive mode
    if (std::getenv("DISPLAY") != nullptr)
    {
        cv::imshow("Foosball Tracking output", frame);
        cv::waitKey(0);
    }

    return 0;
}
