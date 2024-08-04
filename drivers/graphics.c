#include <stdint.h>
#include "../kernel/print.h"

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

void draw_pixel(int x, int y, uint8_t color) {
    uint16_t *framebuffer = (uint16_t *)FRAMEBUFFER_ADDR;
    framebuffer[y * SCREEN_WIDTH + x] = color;
}
