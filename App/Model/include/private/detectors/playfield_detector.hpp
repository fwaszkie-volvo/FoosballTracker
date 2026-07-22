#ifndef FOOSBALL_TRACKER_APP_MODEL_INCLUDE_PRIVATE_DETECTORS_PLAYFIELD_DETECTOR_HPP_
#define FOOSBALL_TRACKER_APP_MODEL_INCLUDE_PRIVATE_DETECTORS_PLAYFIELD_DETECTOR_HPP_

#include <opencv2/opencv.hpp>
#include <vector>

#include "detector.hpp"

class PlayfieldDetector : public Detector
{
  public:
    void Detect(const cv::Mat& frame) override;
    void Draw(cv::Mat& frame) const override;

    const cv::Mat& GetMask() const { return playfield_mask_; }
    bool HasDetection() const { return detected_; }

  private:
    bool ChooseLargestContour(const std::vector<std::vector<cv::Point>>& contours,
                              std::vector<cv::Point>& largest_contour) const;
    std::vector<cv::Point> ApproximatePolygon(const std::vector<cv::Point>& hull) const;

    std::vector<cv::Point> playfield_polygon_;
    cv::Mat playfield_mask_;
    bool detected_ = false;
};

#endif  // FOOSBALL_TRACKER_APP_MODEL_INCLUDE_PRIVATE_DETECTORS_PLAYFIELD_DETECTOR_HPP_
