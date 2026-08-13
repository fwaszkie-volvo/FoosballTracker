#include "save_button.hpp"

#include <gio/gio.h>
#include <glib-object.h>
#include <gtk/gtk.h>

#include "gobject/gclosure.h"
#include "gtk_converters.hpp"

namespace conv
{
SaveButtonData* ToGtkSaveButtonData(gpointer cb_data)
{
    return static_cast<SaveButtonData*>(cb_data);  // NOLINT(bugprone-casting-through-void)
}
}  // namespace conv

void on_save_button_clicked(GtkButton* /*button*/, gpointer user_data)
{
    auto* data        = conv::ToGtkSaveButtonData(user_data);
    GtkWindow* parent = data != nullptr ? data->parent : nullptr;

    GtkWidget* dialog = gtk_file_chooser_dialog_new("Zapisz wynik",
                                                    parent,
                                                    GTK_FILE_CHOOSER_ACTION_SAVE,
                                                    "_Anuluj",
                                                    GTK_RESPONSE_CANCEL,
                                                    "_Zapisz",
                                                    GTK_RESPONSE_ACCEPT,
                                                    nullptr);

    GtkFileFilter* filter_mp4 = gtk_file_filter_new();
    gtk_file_filter_set_name(filter_mp4, "Pliki wideo MP4 (*.mp4)");
    gtk_file_filter_add_pattern(filter_mp4, "*.mp4");
    gtk_file_chooser_add_filter(conv::ToGtkFileChooser(dialog), filter_mp4);

    gtk_file_chooser_set_current_name(conv::ToGtkFileChooser(dialog), "wynik.mp4");
    gtk_window_set_modal(conv::ToGtkWindow(dialog), TRUE);

    // NOLINTNEXTLINE(clang-analyzer-optin.core.EnumCastOutOfRange,bugprone-casting-through-void)
    g_signal_connect(
      dialog,
      "response",
      G_CALLBACK(+[](GtkDialog* dialog_window, gint response, gpointer cb_data)
                 {
                     if (response == GTK_RESPONSE_ACCEPT)
                     {
                         GFile* file =
                           gtk_file_chooser_get_file(conv::ToGtkFileChooser(dialog_window));
                         if (file != nullptr)
                         {
                             char* path                 = g_file_get_path(file);
                             const std::string path_str = path != nullptr ? path : "";
                             g_free(path);
                             g_object_unref(file);

                             auto* btn_data = conv::ToGtkSaveButtonData(cb_data);
                             if (btn_data != nullptr && btn_data->on_save && !path_str.empty())
                             {
                                 btn_data->on_save(path_str);
                             }
                         }
                     }
                     gtk_window_destroy(conv::ToGtkWindow(dialog_window));
                 }),
      data);

    gtk_widget_set_visible(dialog, TRUE);
}
