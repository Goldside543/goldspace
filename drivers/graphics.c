#include <stdint.h>
#include "../kernel/print.h"
#include "../kernel/abs.h"

#define FRAMEBUFFER_ADDR 0xB8000
#define SCREEN_WIDTH  80
#define SCREEN_HEIGHT 25

void init_graphics() {
    print("Loading graphics driver...")
    uint16_t *framebuffer = (uint16_t *)FRAMEBUFFER_ADDR;
    for (int y = 0; y < SCREEN_HEIGHT; ++y) {
        for (int x = 0; x < SCREEN_WIDTH; ++x) {
            framebuffer[y * SCREEN_WIDTH + x] = (0x0F << 8) | ' ';
            print("Graphics driver loaded.")
        }
    }
}

// Helper function to set a pixel
static void set_pixel(int x, int y, uint16_t color) {
    uint16_t *framebuffer = (uint16_t *)FRAMEBUFFER_ADDR;
    framebuffer[y * SCREEN_WIDTH + x] = color;
}

// Draw a rectangle
void draw_rectangle(int x, int y, int width, int height, uint16_t color) {
    for (int i = y; i < y + height; ++i) {
        for (int j = x; j < x + width; ++j) {
            set_pixel(j, i, color);
        }
    }
}

// Draw a line (Bresenham's line algorithm)
void draw_line(int x1, int y1, int x2, int y2, uint16_t color) {
    int dx = abs(x2 - x1);
    int dy = abs(y2 - y1);
    int sx = x1 < x2 ? 1 : -1;
    int sy = y1 < y2 ? 1 : -1;
    int err = dx - dy;

    while (1) {
        set_pixel(x1, y1, color);
        if (x1 == x2 && y1 == y2) break;
        int e2 = err * 2;
        if (e2 > -dy) { err -= dy; x1 += sx; }
        if (e2 < dx) { err += dx; y1 += sy; }
    }
}
