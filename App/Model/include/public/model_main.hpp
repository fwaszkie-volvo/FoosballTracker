#ifndef FOOSBALL_TRACKER_APP_MODEL_INCLUDE_PUBLIC_MODEL_MAIN_HPP_
#define FOOSBALL_TRACKER_APP_MODEL_INCLUDE_PUBLIC_MODEL_MAIN_HPP_

#include <opencv2/core/mat.hpp>
#include <optional>
#include <string>
#include <vector>

#include "generator_types.hpp"
#include "match_storage.hpp"
#include "model_types.hpp"
#include "player.hpp"
#include "ratings_types.hpp"

class ModelMain
{
  public:
    void CalculateFromStream();
    void CalculateFromFile();
    void LoadFile(const std::string& path);
    bool SaveResult(const std::string& destination) const;
    const std::string& GetTempOutputPath() const { return temp_output_path_; }
    const bool CanAnalyzeOfflineFile() const { return can_analyze_offline_file_; }
    std::optional<model::Teams> GenerateTeamsRandom(const generator::Players& players) const;
    std::optional<model::Teams> GenerateTeamsByElo(const generator::Players& players) const;
    bool CreatePlayer(const model::Nickname& nickname);
    bool RecordMatch(const ratings::MatchInput& match);
    std::optional<Player> GetPlayer(const model::Nickname& nickname) const;
    const std::vector<ratings::MatchInput>& GetMatchHistory() const;

  private:
    std::string loaded_file_path_;
    std::string temp_output_path_;
    bool can_analyze_offline_file_{false};

    ratings::MatchStorage storage_{};
};

#endif  // FOOSBALL_TRACKER_APP_MODEL_INCLUDE_PUBLIC_MODEL_MAIN_HPP_
