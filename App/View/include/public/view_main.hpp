#ifndef FOOSBALL_TRACKER_APP_VIEW_INCLUDE_PUBLIC_VIEW_MAIN_HPP_
#define FOOSBALL_TRACKER_APP_VIEW_INCLUDE_PUBLIC_VIEW_MAIN_HPP_

#include <functional>
#include <memory>
#include <mutex>
#include <opencv2/core/mat.hpp>
#include <optional>
#include <string>
#include <vector>

#include "common_types.hpp"

namespace httplib
{
class Request;
class Response;
class Server;
}  // namespace httplib

using common::HttpResult;

// Web-based view: hosts an HTTP API + the built React frontend and exposes the
// same callback-driven interface the Controller previously used with the GTK
// view.
class ViewMain
{
  public:
    ViewMain();
    ~ViewMain();

    int CreateAndRunMain();
    void DrawVideo(const std::string& path);
    void PushLiveFrame(const cv::Mat& frame);
    void ShowAnalProgressDialog();
    void HideAnalProgressDialog();
    void ShowModalCannotAnalyzeOfflineFile();
    void SetOnFileLoaded(std::function<void(const std::string&)> callback);
    void SetOnAnalyseClicked(std::function<void()> callback);
    void SetOnLiveClicked(std::function<void()> callback);
    void SetOnSave(std::function<void(const std::string&)> callback);
    void SetOnCreatePlayer(std::function<bool(const common::Nickname&)> callback);
    void SetOnCheckPlayer(std::function<std::optional<int>(const common::Nickname&)> callback);
    void SetOnGenerateTeams(
      std::function<HttpResult(const std::vector<common::Nickname>&, bool, const std::string&)>
        callback);

  private:
    void RegisterRoutes();
    void HandleOptions(const httplib::Request& req, httplib::Response& res);
    void HandleStatus(const httplib::Request& req, httplib::Response& res);
    void HandleUpload(const httplib::Request& req, httplib::Response& res);
    void HandleAnalyse(const httplib::Request& req, httplib::Response& res);
    void HandleStartLive(const httplib::Request& req, httplib::Response& res);
    void HandleCreatePlayer(const httplib::Request& req, httplib::Response& res);
    void HandleGenerateTeams(const httplib::Request& req, httplib::Response& res);
    void HandleCheckPlayer(const httplib::Request& req, httplib::Response& res);
    void HandleSaveResult(const httplib::Request& req, httplib::Response& res);
    void HandleCurrentVideo(const httplib::Request& req, httplib::Response& res);
    void HandleLiveMjpg(const httplib::Request& req, httplib::Response& res);
    void MountFrontend();

    std::function<void(const std::string&)> on_file_loaded_;
    std::function<void()> on_analyse_clicked_;
    std::function<void()> on_live_clicked_;
    std::function<void(const std::string&)> on_save_;
    std::function<bool(const common::Nickname&)> on_create_player_;
    std::function<std::optional<int>(const common::Nickname&)> on_check_player_;
    std::function<HttpResult(const std::vector<common::Nickname>&, bool, const std::string&)>
      on_generate_teams_;

    std::unique_ptr<httplib::Server> server_;

    mutable std::mutex state_mutex_;
    std::string current_video_path_;
    std::string error_message_;
    bool analysis_in_progress_{false};

    mutable std::mutex live_frame_mutex_;
    std::vector<unsigned char> live_frame_jpeg_;
};

#endif  // FOOSBALL_TRACKER_APP_VIEW_INCLUDE_PUBLIC_VIEW_MAIN_HPP_
