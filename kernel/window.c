#include "../drivers/graphics.h"

// Define a simple window structure
typedef struct {
    int x, y, width, height;
    uint16_t border_color;
    uint16_t fill_color;
} Window;

void draw_window(Window *win) {
    // Draw window border
    draw_rectangle(win->x, win->y, win->width, win->height, win->border_color);
    // Fill window
    draw_rectangle(win->x + 1, win->y + 1, win->width - 2, win->height - 2, win->fill_color);
}
