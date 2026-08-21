#ifndef FOOSBALL_TRACKER_APP_VIEW_INCLUDE_PUBLIC_VIEW_MAIN_HPP_
#define FOOSBALL_TRACKER_APP_VIEW_INCLUDE_PUBLIC_VIEW_MAIN_HPP_

#include <functional>
#include <memory>
#include <mutex>
#include <opencv2/core/mat.hpp>
#include <string>
#include <vector>

namespace httplib
{
class Server;
}  // namespace httplib

// Web-based view: hosts an HTTP API + the built React frontend and exposes the
// same callback-driven interface the Controller previously used with the GTK view.
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

  private:
    void RegisterRoutes();

    std::function<void(const std::string&)> on_file_loaded_;
    std::function<void()> on_analyse_clicked_;
    std::function<void()> on_live_clicked_;
    std::function<void(const std::string&)> on_save_;

    std::unique_ptr<httplib::Server> server_;

    mutable std::mutex state_mutex_;
    std::string current_video_path_;
    std::string error_message_;
    bool analysis_in_progress_{false};

    mutable std::mutex live_frame_mutex_;
    std::vector<unsigned char> live_frame_jpeg_;
};

#endif  // FOOSBALL_TRACKER_APP_VIEW_INCLUDE_PUBLIC_VIEW_MAIN_HPP_
