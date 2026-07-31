#ifndef FOOSBALL_TRACKER_APP_VIEW_INCLUDE_PRIVATE_GTK_CONVERTERS_HPP_
#define FOOSBALL_TRACKER_APP_VIEW_INCLUDE_PRIVATE_GTK_CONVERTERS_HPP_

#include <glib.h>
#include <gtk/gtk.h>

// Then only purpose of this file is to minimize linter errors when converting types
// GTK works on MACROS - which are treated by clang-tidy as void* casts

namespace conv
{
inline GtkWindow* ToGtkWindow(GtkWidget* widget)
{
    return GTK_WINDOW(widget);  // NOLINT(bugprone-casting-through-void)
}

inline GtkWindow* ToGtkWindow(GtkDialog* dialog)
{
    return GTK_WINDOW(dialog);  // NOLINT(bugprone-casting-through-void)
}

inline GtkFileChooser* ToGtkFileChooser(GtkWidget* widget)
{
    return GTK_FILE_CHOOSER(widget);  // NOLINT(bugprone-casting-through-void)
}

inline GtkFileChooser* ToGtkFileChooser(GtkDialog* dialog)
{
    return GTK_FILE_CHOOSER(dialog);  // NOLINT(bugprone-casting-through-void)
}

inline GtkPicture* ToGtkPicture(GtkWidget* widget)
{
    return GTK_PICTURE(widget);  // NOLINT(bugprone-casting-through-void)
}

inline GtkVideo* ToGtkVideo(GtkWidget* widget)
{
    return GTK_VIDEO(widget);  // NOLINT(bugprone-casting-through-void)
}

inline GtkBox* ToGtkBox(GtkWidget* widget)
{
    return GTK_BOX(widget);  // NOLINT(bugprone-casting-through-void)
}

inline GApplication* ToGApplication(GtkApplication* app)
{
    return G_APPLICATION(app);  // NOLINT(bugprone-casting-through-void)
}

inline GtkApplication* ToGtkApplication(void* app)
{
    return GTK_APPLICATION(app);  // NOLINT(bugprone-casting-through-void)
}

}  // namespace conv

#endif  // FOOSBALL_TRACKER_APP_VIEW_INCLUDE_PRIVATE_GTK_CONVERTERS_HPP_