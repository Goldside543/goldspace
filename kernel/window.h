/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef WINDOW_H
#define WINDOW_H

#include "../drivers/graphics.h"

// Define a simple window structure
typedef struct {
    int x, y, width, height;
    uint16_t border_color;
    uint16_t fill_color;
} Window;

// Function prototype for drawing a window
void draw_window(Window *win);

#endif // WINDOW_H
