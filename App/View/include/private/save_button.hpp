#ifndef FOOSBALL_TRACKER_APP_VIEW_INCLUDE_PRIVATE_SAVE_BUTTON_HPP_
#define FOOSBALL_TRACKER_APP_VIEW_INCLUDE_PRIVATE_SAVE_BUTTON_HPP_

#include <glib.h>
#include <gtk/gtk.h>

#include <functional>
#include <string>

struct SaveButtonData
{
    GtkWindow* parent{nullptr};
    std::function<void(const std::string&)> on_save;
};

void on_save_button_clicked(GtkButton* button, gpointer user_data);

#endif  // FOOSBALL_TRACKER_APP_VIEW_INCLUDE_PRIVATE_SAVE_BUTTON_HPP_
