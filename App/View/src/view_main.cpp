#include "view_main.hpp"

#include <gtk/gtk.h>
#include <spdlog/spdlog.h>

#include <opencv2/imgproc.hpp>

namespace
{
struct WindowState
{
    std::optional<cv::Mat> frame_bgr;
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

void on_activate(GtkApplication* app, gpointer user_data)
{
    const auto* state = static_cast<const WindowState*>(user_data);

    GtkWindow* gtk_window = ToGtkWindow(gtk_application_window_new(app));
    gtk_window_set_title(gtk_window, "Foosball Tracker");
    gtk_window_set_default_size(gtk_window, 800, 600);

    GtkWidget* content = nullptr;
    if (state != nullptr && state->frame_bgr.has_value())
    {
        content = CreateImageWidget(state->frame_bgr.value());
    }
    else
    {
        content = gtk_label_new("Model nie zwrocil zadnej ramki.");
    }

    gtk_window_set_child(gtk_window, content);

    gtk_window_present(gtk_window);
}

}  // namespace

void ViewMain::Draw(const std::optional<cv::Mat>& frame)
{
    WindowState state{frame};
    GtkApplication* app = gtk_application_new("com.foosballtracker.app", G_APPLICATION_FLAGS_NONE);

    // NOLINTNEXTLINE(clang-analyzer-optin.core.EnumCastOutOfRange,bugprone-casting-through-void)
    g_signal_connect(app, "activate", G_CALLBACK(on_activate), &state);

    // NOLINTNEXTLINE(bugprone-casting-through-void)
    g_application_run(G_APPLICATION(app), 0, nullptr);
    g_object_unref(app);
}
