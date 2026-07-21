#include "model_main.hpp"

#include <filesystem>
#include <opencv2/opencv.hpp>

#include "detector.hpp"
#include "detector_config.hpp"

void ModelMain::Calculate()
{
    const std::filesystem::path input_image_path =
      std::filesystem::absolute(detector_config::kInputImagePath);

    cv::Mat frame = cv::imread(input_image_path.string(), cv::IMREAD_COLOR);
    if (frame.empty())
    {
        std::cerr << "Failed to load input image: " << input_image_path.string() << "\n";
        return;
    }

    detect_ball(frame);

    cv::imwrite(detector_config::kOutputImagePath, frame);

    // Only display if running in interactive mode
    if (std::getenv("DISPLAY") != nullptr)
    {
        cv::imshow("Foosball Tracking output", frame);
        cv::waitKey(0);
    }
}