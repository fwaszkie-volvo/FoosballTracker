#ifndef FOOSBALL_TRACKER_APP_MODEL_INCLUDE_PRIVATE_DETECTORS_PLAYFIELD_DETECTOR_HPP_
#define FOOSBALL_TRACKER_APP_MODEL_INCLUDE_PRIVATE_DETECTORS_PLAYFIELD_DETECTOR_HPP_

#include <opencv2/core/mat.hpp>
#include <vector>

#include "detector.hpp"
#include "detector_types.hpp"

class PlayfieldDetector : public Detector
{
  public:
    void Detect(const cv::Mat& frame) override;
    void Draw(cv::Mat& frame) const override;

    const cv::Mat& GetMask() const { return playfield_mask_; }
    bool HasDetection() const { return detected_; }

  private:
    bool ChooseLargestContour(const std::vector<Contour>& contours, Contour& largest_contour) const;
    Contour ApproximatePolygon(const Contour& hull) const;

    Contour playfield_polygon_{};
    cv::Mat playfield_mask_{};
    bool detected_{};
};

#endif  // FOOSBALL_TRACKER_APP_MODEL_INCLUDE_PRIVATE_DETECTORS_PLAYFIELD_DETECTOR_HPP_
