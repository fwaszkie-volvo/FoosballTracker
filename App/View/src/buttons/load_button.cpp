#include "load_button.hpp"

#include <gio/gio.h>
#include <glib-object.h>
#include <gtk/gtk.h>
#include <spdlog/spdlog.h>

#include "gobject/gclosure.h"

namespace
{
GtkWidget* GetDialogButton(GtkWindow* parent)
{
    return gtk_file_chooser_dialog_new("Wybierz plik wideo",
                                       parent,
                                       GTK_FILE_CHOOSER_ACTION_OPEN,
                                       "_Anuluj",
                                       GTK_RESPONSE_CANCEL,
                                       "_Otworz",
                                       GTK_RESPONSE_ACCEPT,
                                       nullptr);
}

GtkFileFilter* GetMP4Filter()
{
    GtkFileFilter* filter = gtk_file_filter_new();
    gtk_file_filter_set_name(filter, "Pliki wideo MP4 (*.mp4)");
    gtk_file_filter_add_pattern(filter, "*.mp4");
    return filter;
}

GtkFileFilter* GetFilterAllFiles()
{
    GtkFileFilter* filter = gtk_file_filter_new();
    gtk_file_filter_set_name(filter, "Wszystkie pliki");
    gtk_file_filter_add_pattern(filter, "*");
    return filter;
}

void OnDialogResponse(GtkDialog* d, gint response, gpointer cb_data)
{
    if (response == GTK_RESPONSE_ACCEPT)
    {
        GFile* file = gtk_file_chooser_get_file(GTK_FILE_CHOOSER(d));
        if (file != nullptr)
        {
            char* path = g_file_get_path(file);
            const std::string path_str = path != nullptr ? path : "";
            spdlog::info("LOAD: wybrano plik: {}", path_str);
            g_free(path);
            g_object_unref(file);

            auto* btn_data =  // NOLINT(bugprone-casting-through-void)
              static_cast<LoadButtonData*>(cb_data);
            if (btn_data != nullptr && btn_data->on_file_loaded && !path_str.empty())
            {
                btn_data->on_file_loaded(path_str);
            }
        }
    }
    gtk_window_destroy(GTK_WINDOW(d));
}
}  // namespace

void on_load_button_clicked(GtkButton* /*button*/, gpointer user_data)
{
    auto* data = static_cast<LoadButtonData*>(user_data);  // NOLINT(bugprone-casting-through-void)
    GtkWindow* parent = data != nullptr ? data->parent : nullptr;

    GtkWidget* dialog = GetDialogButton(parent);

    GtkFileFilter* filter_mp4 = GetMP4Filter();
    gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(dialog), filter_mp4);

    GtkFileFilter* filter_all = GetFilterAllFiles();
    gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(dialog), filter_all);

    gtk_window_set_modal(GTK_WINDOW(dialog), TRUE);

    g_signal_connect(dialog,  // NOLINT(bugprone-casting-through-void)
                     "response",
                     G_CALLBACK(OnDialogResponse),
                     data);

    gtk_widget_set_visible(dialog, TRUE);
}
