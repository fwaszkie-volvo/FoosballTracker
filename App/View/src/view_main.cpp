#include "view_main.hpp"

#include <httplib.h>
#include <spdlog/spdlog.h>

#include <chrono>
#include <filesystem>
#include <fstream>
#include <opencv2/imgcodecs.hpp>
#include <sstream>
#include <string>
#include <thread>
#include <utility>

namespace
{
constexpr int kPort         = 8080;
constexpr const char* kHost = "0.0.0.0";

std::string JsonEscape(const std::string& value)
{
    std::string escaped;
    escaped.reserve(value.size());
    for (char c : value)
    {
        switch (c)
        {
            case '"':
                escaped += "\\\"";
                break;
            case '\\':
                escaped += "\\\\";
                break;
            case '\n':
                escaped += "\\n";
                break;
            case '\r':
                escaped += "\\r";
                break;
            default:
                escaped += c;
        }
    }
    return escaped;
}

std::string JsonStringOrNull(const std::string& value)
{
    return value.empty() ? "null" : "\"" + JsonEscape(value) + "\"";
}

}  // namespace

ViewMain::ViewMain() : server_{std::make_unique<httplib::Server>()} {}

ViewMain::~ViewMain() = default;

void ViewMain::DrawVideo(const std::string& path)
{
    const std::lock_guard<std::mutex> lock{state_mutex_};
    current_video_path_ = path;
}

void ViewMain::PushLiveFrame(const cv::Mat& frame)
{
    if (frame.empty())
    {
        return;
    }

    std::vector<unsigned char> encoded;
    if (!cv::imencode(".jpg", frame, encoded))
    {
        return;
    }

    const std::lock_guard<std::mutex> lock{live_frame_mutex_};
    live_frame_jpeg_ = std::move(encoded);
}

void ViewMain::ShowAnalProgressDialog()
{
    const std::lock_guard<std::mutex> lock{state_mutex_};
    analysis_in_progress_ = true;
}

void ViewMain::HideAnalProgressDialog()
{
    const std::lock_guard<std::mutex> lock{state_mutex_};
    analysis_in_progress_ = false;
}

void ViewMain::ShowModalCannotAnalyzeOfflineFile()
{
    const std::lock_guard<std::mutex> lock{state_mutex_};
    error_message_ = "Nie mozna analizowac pliku offline. Zaladuj plik i sprobuj ponownie.";
}

void ViewMain::SetOnFileLoaded(std::function<void(const std::string&)> callback)
{
    on_file_loaded_ = std::move(callback);
}

void ViewMain::SetOnAnalyseClicked(std::function<void()> callback)
{
    on_analyse_clicked_ = std::move(callback);
}

void ViewMain::SetOnLiveClicked(std::function<void()> callback)
{
    on_live_clicked_ = std::move(callback);
}

void ViewMain::SetOnSave(std::function<void(const std::string&)> callback)
{
    on_save_ = std::move(callback);
}

void ViewMain::RegisterRoutes()
{
    server_->set_default_headers({{"Access-Control-Allow-Origin", "*"}});

    server_->Options(".*",
                     [](const httplib::Request&, httplib::Response& res)
                     {
                         res.set_header("Access-Control-Allow-Methods", "GET, POST, OPTIONS");
                         res.set_header("Access-Control-Allow-Headers", "Content-Type");
                         res.status = 204;
                     });

    server_->Get("/api/status",
                 [this](const httplib::Request&, httplib::Response& res)
                 {
                     std::string video_url;
                     std::string error;
                     bool analyzing = false;
                     {
                         const std::lock_guard<std::mutex> lock{state_mutex_};
                         if (!current_video_path_.empty())
                         {
                             video_url = "/media/current";
                         }
                         error     = error_message_;
                         analyzing = analysis_in_progress_;
                         error_message_.clear();
                     }

                     std::ostringstream body;
                     body << "{\"analyzing\":" << (analyzing ? "true" : "false")
                          << ",\"error\":" << JsonStringOrNull(error)
                          << ",\"videoUrl\":" << JsonStringOrNull(video_url) << "}";
                     res.set_content(body.str(), "application/json");
                 });

    server_->Post(
      "/api/load",
      [this](const httplib::Request& req, httplib::Response& res)
      {
          if (!req.has_file("file"))
          {
              res.status = 400;
              res.set_content("{\"error\":\"Brak pliku w polu 'file'.\"}", "application/json");
              return;
          }

          const auto& file     = req.get_file_value("file");
          const auto extension = std::filesystem::path(file.filename).extension().string();
          const auto dest_path =
            std::filesystem::temp_directory_path() /
            ("foosball_upload_" +
             std::to_string(std::chrono::steady_clock::now().time_since_epoch().count()) +
             (extension.empty() ? ".mp4" : extension));

          std::ofstream out{dest_path, std::ios::binary};
          out << file.content;
          out.close();

          if (on_file_loaded_)
          {
              on_file_loaded_(dest_path.string());
          }

          res.set_content("{\"ok\":true}", "application/json");
      });

    server_->Post("/api/analyse",
                  [this](const httplib::Request&, httplib::Response& res)
                  {
                      if (on_analyse_clicked_)
                      {
                          on_analyse_clicked_();
                      }
                      res.set_content("{\"ok\":true}", "application/json");
                  });

    server_->Post("/api/live",
                  [this](const httplib::Request&, httplib::Response& res)
                  {
                      if (on_live_clicked_)
                      {
                          on_live_clicked_();
                      }
                      res.set_content("{\"ok\":true}", "application/json");
                  });

    server_->Get(
      "/api/save",
      [this](const httplib::Request&, httplib::Response& res)
      {
          if (!on_save_)
          {
              res.status = 500;
              res.set_content("{\"error\":\"Zapis nie jest dostepny.\"}", "application/json");
              return;
          }

          const auto temp_path =
            std::filesystem::temp_directory_path() /
            ("foosball_result_" +
             std::to_string(std::chrono::steady_clock::now().time_since_epoch().count()) + ".mp4");

          on_save_(temp_path.string());

          if (!std::filesystem::exists(temp_path))
          {
              res.status = 404;
              res.set_content("{\"error\":\"Nie udalo sie przygotowac pliku wynikowego.\"}",
                              "application/json");
              return;
          }

          const auto file_size = std::filesystem::file_size(temp_path);
          auto stream          = std::make_shared<std::ifstream>(temp_path, std::ios::binary);
          res.set_header("Content-Disposition", "attachment; filename=\"foosball_result.mp4\"");
          res.set_content_provider(
            file_size,
            "video/mp4",
            [stream](size_t offset, size_t length, httplib::DataSink& sink) -> bool
            {
                std::vector<char> buffer(length);
                stream->seekg(static_cast<std::streamoff>(offset));
                stream->read(buffer.data(), static_cast<std::streamsize>(length));
                sink.write(buffer.data(), length);
                return true;
            },
            [temp_path](bool /*success*/)
            {
                std::error_code ec;
                std::filesystem::remove(temp_path, ec);
            });
      });

    server_->Get("/media/current",
                 [this](const httplib::Request&, httplib::Response& res)
                 {
                     std::string path;
                     {
                         const std::lock_guard<std::mutex> lock{state_mutex_};
                         path = current_video_path_;
                     }

                     if (path.empty() || !std::filesystem::exists(path))
                     {
                         res.status = 404;
                         return;
                     }

                     const auto file_size = std::filesystem::file_size(path);
                     auto stream          = std::make_shared<std::ifstream>(path, std::ios::binary);
                     res.set_content_provider(
                       file_size,
                       "video/mp4",
                       [stream](size_t offset, size_t length, httplib::DataSink& sink) -> bool
                       {
                           std::vector<char> buffer(length);
                           stream->seekg(static_cast<std::streamoff>(offset));
                           stream->read(buffer.data(), static_cast<std::streamsize>(length));
                           sink.write(buffer.data(), length);
                           return true;
                       });
                 });

    server_->Get("/media/live.mjpg",
                 [this](const httplib::Request&, httplib::Response& res)
                 {
                     static const std::string kBoundary = "foosballframe";
                     res.set_chunked_content_provider(
                       "multipart/x-mixed-replace; boundary=" + kBoundary,
                       [this](size_t /*offset*/, httplib::DataSink& sink) -> bool
                       {
                           std::vector<unsigned char> frame;
                           {
                               const std::lock_guard<std::mutex> lock{live_frame_mutex_};
                               frame = live_frame_jpeg_;
                           }

                           if (!frame.empty())
                           {
                               std::ostringstream header;
                               header << "--" << kBoundary << "\r\n"
                                      << "Content-Type: image/jpeg\r\n"
                                      << "Content-Length: " << frame.size() << "\r\n\r\n";
                               const auto header_str = header.str();
                               sink.write(header_str.data(), header_str.size());
                               sink.write(reinterpret_cast<const char*>(frame.data()),
                                          frame.size());
                               sink.write("\r\n", 2);
                           }

                           std::this_thread::sleep_for(std::chrono::milliseconds(100));
                           return true;
                       });
                 });

    const char* frontend_dir = FOOSBALL_FRONTEND_BUILD_DIR;
    if (std::filesystem::exists(frontend_dir))
    {
        server_->set_mount_point("/", frontend_dir);
    }
    else
    {
        spdlog::warn("Katalog build reactowego frontendu nie istnieje: {}", frontend_dir);
    }
}

int ViewMain::CreateAndRunMain()
{
    RegisterRoutes();
    spdlog::info("Uruchamiam serwer HTTP na {}:{}", kHost, kPort);
    if (!server_->listen(kHost, kPort))
    {
        spdlog::error("Nie udalo sie uruchomic serwera HTTP na porcie {}", kPort);
        return 1;
    }
    return 0;
}
