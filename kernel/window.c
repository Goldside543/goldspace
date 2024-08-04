#include "../drivers/graphics.h"
#include "window.h"

void draw_window(Window *win) {
    // Draw window border
    draw_rectangle(win->x, win->y, win->width, win->height, win->border_color);
    // Fill window
    draw_rectangle(win->x + 1, win->y + 1, win->width - 2, win->height - 2, win->fill_color);
}
