#ifndef FOOSBALL_TRACKER_APP_MODEL_INCLUDE_PUBLIC_MODEL_MAIN_HPP_
#define FOOSBALL_TRACKER_APP_MODEL_INCLUDE_PUBLIC_MODEL_MAIN_HPP_

#include <opencv2/core/mat.hpp>
#include <optional>
#include <string>

#include "generator_types.hpp"

class ModelMain
{
  public:
    void CalculateFromStream();
    void CalculateFromFile();
    std::optional<generator::TeamDrawResult> GenerateTeams(const generator::Players& players) const;
    void LoadFile(const std::string& path);
    bool SaveResult(const std::string& destination) const;
    const std::string& GetTempOutputPath() const { return temp_output_path_; }

  private:
    std::string loaded_file_path_;
    std::string temp_output_path_;
};

#endif  // FOOSBALL_TRACKER_APP_MODEL_INCLUDE_PUBLIC_MODEL_MAIN_HPP_
