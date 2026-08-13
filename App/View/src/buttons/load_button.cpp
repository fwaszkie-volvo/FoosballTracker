#include "load_button.hpp"

#include <gio/gio.h>
#include <glib-object.h>
#include <gtk/gtk.h>
#include <spdlog/spdlog.h>

#include "gobject/gclosure.h"
#include "gtk_converters.hpp"

namespace conv
{
LoadButtonData* ToGtkLoadButtonData(gpointer cb_data)
{
    return static_cast<LoadButtonData*>(cb_data);  // NOLINT(bugprone-casting-through-void)
}
}  // namespace conv

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

void OnDialogResponse(GtkDialog* dialog_window, gint response, gpointer cb_data)
{
    if (response == GTK_RESPONSE_ACCEPT)
    {
        GFile* file = gtk_file_chooser_get_file(conv::ToGtkFileChooser(dialog_window));
        if (file != nullptr)
        {
            char* path = g_file_get_path(file);
            const std::string path_str = path != nullptr ? path : "";
            spdlog::info("LOAD: wybrano plik: {}", path_str);
            g_free(path);
            g_object_unref(file);

            auto* btn_data = conv::ToGtkLoadButtonData(cb_data);
            if (btn_data != nullptr && btn_data->on_file_loaded && !path_str.empty())
            {
                btn_data->on_file_loaded(path_str);
            }
        }
    }
    gtk_window_destroy(conv::ToGtkWindow(dialog_window));
}
}  // namespace

void on_load_button_clicked(GtkButton* /*button*/, gpointer user_data)
{
    auto* data = conv::ToGtkLoadButtonData(user_data);
    GtkWindow* parent = data != nullptr ? data->parent : nullptr;

    GtkWidget* dialog_button = GetDialogButton(parent);

    GtkFileFilter* filter_mp4 = GetMP4Filter();
    gtk_file_chooser_add_filter(conv::ToGtkFileChooser(dialog_button), filter_mp4);

    gtk_window_set_modal(conv::ToGtkWindow(dialog_button), TRUE);

    // NOLINTNEXTLINE(clang-analyzer-optin.core.EnumCastOutOfRange,bugprone-casting-through-void)
    g_signal_connect(dialog_button, "response", G_CALLBACK(OnDialogResponse), data);

    gtk_window_present(conv::ToGtkWindow(dialog_button));
}
