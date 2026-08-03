#include "live_button.hpp"

#include <functional>

void on_live_button_clicked(GtkButton* /*button*/, gpointer user_data)
{
    // NOLINTNEXTLINE(bugprone-casting-through-void)
    const auto* callback = static_cast<std::function<void()>*>(user_data);
    if (callback != nullptr && *callback)
    {
        std::invoke(*callback);
    }
}
