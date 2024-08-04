#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <stdint.h>

// Function prototypes
void init_graphics(void);
void draw_rectangle(int x, int y, int width, int height, uint16_t color);
void draw_line(int x1, int y1, int x2, int y2, uint16_t color);

// Helper function for setting a pixel, used internally
static void set_pixel(int x, int y, uint16_t color);

#endif // GRAPHICS_H
