#include "view_main.hpp"

#include <gdk-pixbuf/gdk-pixbuf.h>
#include <gdk/gdk.h>
#include <gio/gio.h>
#include <glib-object.h>
#include <glib.h>
#include <gtk/gtk.h>
#include <spdlog/spdlog.h>
#include <stddef.h>

#include <functional>
#include <opencv2/core/mat.inl.hpp>
#include <opencv2/imgproc.hpp>
#include <string>
#include <type_traits>
#include <utility>

#include "anal_button.hpp"
#include "gobject/gclosure.h"
#include "live_button.hpp"
#include "load_button.hpp"
#include "save_button.hpp"

namespace
{
const std::string kAnalInProgress = "ANAL IN PROGRESS";

struct WindowState
{
    std::function<void(const std::string&)> on_file_loaded;
    std::function<void()> on_analyse_clicked;
    std::function<void()> on_live_clicked;
    std::function<void(const std::string&)> on_save;
    LoadButtonData load_button_data{};
    SaveButtonData save_button_data{};
    ViewMain* view_main{nullptr};
};

GtkWindow* ToGtkWindow(GtkWidget* widget)
{
    return GTK_WINDOW(widget);  // NOLINT(bugprone-casting-through-void)
}

GtkWidget* CreateImageWidget(const cv::Mat& frame)
{
    if (frame.empty())
    {
        return gtk_label_new("Brak danych ramki do wyswietlenia.");
    }

    cv::Mat converted;
    bool has_alpha = false;

    if (frame.channels() == 3)
    {
        cv::cvtColor(frame, converted, cv::COLOR_BGR2RGB);
    }
    else if (frame.channels() == 4)
    {
        cv::cvtColor(frame, converted, cv::COLOR_BGRA2RGBA);
        has_alpha = true;
    }
    else if (frame.channels() == 1)
    {
        cv::cvtColor(frame, converted, cv::COLOR_GRAY2RGB);
    }
    else
    {
        return gtk_label_new("Nieobslugiwany format ramki.");
    }

    GBytes* bytes =
      g_bytes_new(converted.data, converted.total() * static_cast<size_t>(converted.elemSize()));
    GdkPixbuf* pixbuf = gdk_pixbuf_new_from_bytes(bytes,
                                                  GDK_COLORSPACE_RGB,
                                                  has_alpha,
                                                  8,
                                                  converted.cols,
                                                  converted.rows,
                                                  static_cast<int>(converted.step));
    GdkTexture* texture = gdk_texture_new_for_pixbuf(pixbuf);
    GtkWidget* picture = gtk_picture_new_for_paintable(GDK_PAINTABLE(texture));
    gtk_picture_set_can_shrink(GTK_PICTURE(picture), TRUE);

    g_object_unref(texture);
    g_object_unref(pixbuf);
    g_bytes_unref(bytes);

    return picture;
}

GtkWidget* CreateVideoWidget(const std::string& path)
{
    GtkWidget* video = gtk_video_new_for_filename(path.c_str());
    gtk_video_set_autoplay(GTK_VIDEO(video), TRUE);  // NOLINT(bugprone-casting-through-void)
    gtk_video_set_loop(GTK_VIDEO(video), FALSE);     // NOLINT(bugprone-casting-through-void)

    // autoplay nie startuje przed realizacją widgetu — wymuszamy play po realize
    g_signal_connect(  // NOLINT(bugprone-casting-through-void)
      video,
      "realize",
      G_CALLBACK(+[](GtkWidget* w, gpointer)
                 {
                     auto* stream = gtk_video_get_media_stream(
                       GTK_VIDEO(w));  // NOLINT(bugprone-casting-through-void)
                     if (stream != nullptr)
                     {
                         gtk_media_stream_play(stream);
                     }
                 }),
      nullptr);

    return video;
}

void on_activate(GtkApplication* app, gpointer user_data)
{
    spdlog::info("on_activate: START");
    auto* state = static_cast<WindowState*>(user_data);  // NOLINT(bugprone-casting-through-void)

    GtkWindow* gtk_window = ToGtkWindow(gtk_application_window_new(app));
    gtk_window_set_title(gtk_window, "Foosball Tracker");
    gtk_window_set_default_size(gtk_window, 800, 600);

    if (state != nullptr)
    {
        state->load_button_data = {gtk_window, state->on_file_loaded};
        state->save_button_data = {gtk_window, state->on_save};
    }

    GtkWidget* vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);

    GtkWidget* button_bar = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 4);
    gtk_widget_set_margin_start(button_bar, 4);
    gtk_widget_set_margin_end(button_bar, 4);
    gtk_widget_set_margin_top(button_bar, 4);
    gtk_widget_set_margin_bottom(button_bar, 4);

    GtkWidget* btn_live = gtk_button_new_with_label("LIVE");
    GtkWidget* btn_load = gtk_button_new_with_label("LOAD");
    GtkWidget* btn_anal = gtk_button_new_with_label("ANAL");
    GtkWidget* btn_save = gtk_button_new_with_label("SAVE");
    // NOLINTNEXTLINE(bugprone-casting-through-void)
    g_signal_connect(btn_live,
                     "clicked",
                     G_CALLBACK(on_live_button_clicked),
                     state != nullptr ? &state->on_live_clicked : nullptr);
    // NOLINTNEXTLINE(bugprone-casting-through-void)
    g_signal_connect(btn_load,
                     "clicked",
                     G_CALLBACK(on_load_button_clicked),
                     state != nullptr ? &state->load_button_data : nullptr);
    // NOLINTNEXTLINE(bugprone-casting-through-void)
    g_signal_connect(btn_anal,
                     "clicked",
                     G_CALLBACK(on_anal_button_clicked),
                     state != nullptr ? &state->on_analyse_clicked : nullptr);
    // NOLINTNEXTLINE(bugprone-casting-through-void)
    g_signal_connect(btn_save,
                     "clicked",
                     G_CALLBACK(on_save_button_clicked),
                     state != nullptr ? &state->save_button_data : nullptr);
    gtk_box_append(GTK_BOX(button_bar), btn_live);
    gtk_box_append(GTK_BOX(button_bar), btn_load);
    gtk_box_append(GTK_BOX(button_bar), btn_anal);
    gtk_box_append(GTK_BOX(button_bar), btn_save);

    gtk_box_append(GTK_BOX(vbox), button_bar);

    GtkWidget* content = gtk_label_new("Przygotuj i załaduj do ANALA.");
    gtk_widget_set_vexpand(content, TRUE);
    gtk_box_append(GTK_BOX(vbox), content);

    gtk_window_set_child(gtk_window, vbox);

    if (state != nullptr && state->view_main != nullptr)
    {
        state->view_main->SetContentVbox(vbox);
    }

    gtk_window_present(gtk_window);
    spdlog::info("on_activate: END");
}

}  // namespace

void ViewMain::ShowProgressDialog(const std::string& message)
{
    auto* dialog = gtk_window_new();
    gtk_window_set_title(GTK_WINDOW(dialog), "");         // NOLINT(bugprone-casting-through-void)
    gtk_window_set_modal(GTK_WINDOW(dialog), TRUE);       // NOLINT(bugprone-casting-through-void)
    gtk_window_set_resizable(GTK_WINDOW(dialog), FALSE);  // NOLINT(bugprone-casting-through-void)
    gtk_window_set_deletable(GTK_WINDOW(dialog), FALSE);  // NOLINT(bugprone-casting-through-void)

    if (gtk_app_ != nullptr)
    {
        GList* windows = gtk_application_get_windows(  // NOLINT(bugprone-casting-through-void)
          GTK_APPLICATION(gtk_app_));
        if (windows != nullptr)
        {
            gtk_window_set_transient_for(
              GTK_WINDOW(dialog),          // NOLINT(bugprone-casting-through-void)
              GTK_WINDOW(windows->data));  // NOLINT(bugprone-casting-through-void)
        }
    }

    GtkWidget* label = gtk_label_new(message.c_str());
    gtk_widget_set_margin_start(label, 32);
    gtk_widget_set_margin_end(label, 32);
    gtk_widget_set_margin_top(label, 24);
    gtk_widget_set_margin_bottom(label, 24);
    gtk_window_set_child(GTK_WINDOW(dialog), label);  // NOLINT(bugprone-casting-through-void)

    progress_dialog_ = dialog;
    gtk_widget_set_visible(dialog, TRUE);
}

void ViewMain::HideProgressDialog()
{
    if (progress_dialog_ != nullptr)
    {
        gtk_window_destroy(  // NOLINT(bugprone-casting-through-void)
          GTK_WINDOW(static_cast<GtkWidget*>(progress_dialog_)));
        progress_dialog_ = nullptr;
    }
}

void ViewMain::CreateAnalWindowInProgress(std::function<void()> on_done)
{
    ShowProgressDialog(kAnalInProgress);

    struct Context
    {
        ViewMain* view;
        std::function<void()> on_done;
    };
    auto* ctx = new Context{this, std::move(on_done)};

    g_idle_add(  // NOLINT(bugprone-casting-through-void)
      G_SOURCE_FUNC(+[](gpointer data) -> gboolean
                    {
                        auto* local_ctx =
                          static_cast<Context*>(data);  // NOLINT(bugprone-casting-through-void)
                        local_ctx->view->HideProgressDialog();
                        local_ctx->on_done();
                        delete local_ctx;
                        return G_SOURCE_REMOVE;
                    }),
      ctx);
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

void ViewMain::UpdateContent(const std::optional<cv::Mat>& frame)
{
    auto* vbox = static_cast<GtkWidget*>(gtk_content_vbox_);
    if (vbox == nullptr)
    {
        return;
    }

    // The content widget is the second child of vbox (after button_bar).
    GtkWidget* button_bar = gtk_widget_get_first_child(vbox);
    GtkWidget* old_content =
      button_bar != nullptr ? gtk_widget_get_next_sibling(button_bar) : nullptr;
    if (old_content != nullptr)
    {
        gtk_box_remove(GTK_BOX(vbox), old_content);
    }

    GtkWidget* new_content = frame.has_value() ? CreateImageWidget(frame.value())
                                               : gtk_label_new("Model nie zwrocil zadnej ramki.");
    gtk_widget_set_vexpand(new_content, TRUE);
    gtk_box_append(GTK_BOX(vbox), new_content);
}

void ViewMain::UpdateContentWithVideo(const std::string& path)
{
    auto* vbox = static_cast<GtkWidget*>(gtk_content_vbox_);
    if (vbox == nullptr)
    {
        return;
    }

    GtkWidget* button_bar = gtk_widget_get_first_child(vbox);
    GtkWidget* old_content =
      button_bar != nullptr ? gtk_widget_get_next_sibling(button_bar) : nullptr;
    if (old_content != nullptr)
    {
        gtk_box_remove(GTK_BOX(vbox), old_content);
    }

    GtkWidget* video = CreateVideoWidget(path);
    gtk_widget_set_vexpand(video, TRUE);
    gtk_box_append(GTK_BOX(vbox), video);
}

void ViewMain::SetOnSave(std::function<void(const std::string&)> callback)
{
    on_save_ = std::move(callback);
}

void ViewMain::DrawVideo(const std::string& path)
{
    if (gtk_app_ == nullptr)
    {
        spdlog::warn("DrawVideo: aplikacja GTK nie jest uruchomiona");
        return;
    }
    UpdateContentWithVideo(path);
}

int ViewMain::CreateAndRunMain(int argc, char* argv[])
{
    spdlog::info("DrawInitial: start");
    WindowState state{
      on_file_loaded_, on_analyse_clicked_, on_live_clicked_, on_save_, {}, {}, this};
    spdlog::info("DrawInitial: 1");
    auto* app = gtk_application_new("foosballtracker.app", G_APPLICATION_FLAGS_NONE);  // NOLINT
    spdlog::info("DrawInitial: 2");
    gtk_app_ = app;
    spdlog::info("DrawInitial: gtk_app_ == nullptr {}", gtk_app_ == nullptr ? "true" : "false");
    spdlog::info("DrawInitial: 3");

    // NOLINTNEXTLINE(clang-analyzer-optin.core.EnumCastOutOfRange,bugprone-casting-through-void)
    g_signal_connect(app, "activate", G_CALLBACK(on_activate), &state);
    spdlog::info("DrawInitial: signal connected");

    // NOLINTNEXTLINE(bugprone-casting-through-void)
    spdlog::info("DrawInitial: 4");
    int ret_val = g_application_run(G_APPLICATION(app), argc, argv);
    spdlog::info("DrawInitial: 5");
    g_object_unref(app);
    spdlog::info("DrawInitial: 6");
    gtk_app_ = nullptr;
    spdlog::info("DrawInitial: 7");
    gtk_content_vbox_ = nullptr;
    spdlog::info("DrawInitial: stop");
    return ret_val;
}
