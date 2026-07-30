#include "save_button.hpp"

#include <gio/gio.h>
#include <glib-object.h>
#include <gtk/gtk.h>

#include "gobject/gclosure.h"

void on_save_button_clicked(GtkButton* /*button*/, gpointer user_data)
{
    auto* data = static_cast<SaveButtonData*>(user_data);  // NOLINT(bugprone-casting-through-void)
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
    gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(dialog), filter_mp4);

    gtk_file_chooser_set_current_name(GTK_FILE_CHOOSER(dialog), "wynik.mp4");
    gtk_window_set_modal(GTK_WINDOW(dialog), TRUE);

    g_signal_connect(
      dialog,  // NOLINT(bugprone-casting-through-void)
      "response",
      G_CALLBACK(+[](GtkDialog* d, gint response, gpointer cb_data)
                 {
                     if (response == GTK_RESPONSE_ACCEPT)
                     {
                         GFile* file = gtk_file_chooser_get_file(GTK_FILE_CHOOSER(d));
                         if (file != nullptr)
                         {
                             char* path = g_file_get_path(file);
                             const std::string path_str = path != nullptr ? path : "";
                             g_free(path);
                             g_object_unref(file);

                             auto* btn_data =  // NOLINT(bugprone-casting-through-void)
                               static_cast<SaveButtonData*>(cb_data);
                             if (btn_data != nullptr && btn_data->on_save && !path_str.empty())
                             {
                                 btn_data->on_save(path_str);
                             }
                         }
                     }
                     gtk_window_destroy(GTK_WINDOW(d));
                 }),
      data);

    gtk_widget_set_visible(dialog, TRUE);
}
