#ifndef FOOSBALL_TRACKER_APP_VIEW_INCLUDE_PUBLIC_VIEW_MAIN_HPP_
#define FOOSBALL_TRACKER_APP_VIEW_INCLUDE_PUBLIC_VIEW_MAIN_HPP_

#include <functional>
#include <opencv2/core/mat.hpp>
#include <optional>
#include <string>

class ViewMain
{
  public:
    void Draw(const std::optional<cv::Mat>& frame);
    void DrawVideo(const std::string& path);
    void RunWithProgress(const std::string& message,
                         std::function<void()> background_work,
                         std::function<void()> on_done);
    void SetOnFileLoaded(std::function<void(const std::string&)> callback);
    void SetOnAnalyseClicked(std::function<void()> callback);
    void SetOnLiveClicked(std::function<void()> callback);
    void SetOnSave(std::function<void(const std::string&)> callback);

    void SetContentVbox(void* vbox) { gtk_content_vbox_ = vbox; }

  private:
    void UpdateContent(const std::optional<cv::Mat>& frame);
    void UpdateContentWithVideo(const std::string& path);
    void ShowProgressDialog(const std::string& message);
    void HideProgressDialog();

    std::function<void(const std::string&)> on_file_loaded_;
    std::function<void()> on_analyse_clicked_;
    std::function<void()> on_live_clicked_;
    std::function<void(const std::string&)> on_save_;
    void* gtk_app_{nullptr};           // GtkApplication*
    void* gtk_content_vbox_{nullptr};  // GtkWidget*
    void* progress_dialog_{nullptr};   // GtkWidget*
};

#endif  // FOOSBALL_TRACKER_APP_VIEW_INCLUDE_PUBLIC_VIEW_MAIN_HPP_
