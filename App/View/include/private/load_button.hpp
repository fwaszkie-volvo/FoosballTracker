#ifndef FOOSBALL_TRACKER_APP_VIEW_INCLUDE_PRIVATE_LOAD_BUTTON_HPP_
#define FOOSBALL_TRACKER_APP_VIEW_INCLUDE_PRIVATE_LOAD_BUTTON_HPP_

#include <glib.h>
#include <gtk/gtk.h>

#include <functional>
#include <string>

struct LoadButtonData
{
    GtkWindow* parent{nullptr};
    std::function<void(const std::string&)> on_file_loaded;
};

void on_load_button_clicked(GtkButton* button, gpointer user_data);

#endif  // FOOSBALL_TRACKER_APP_VIEW_INCLUDE_PRIVATE_LOAD_BUTTON_HPP_
