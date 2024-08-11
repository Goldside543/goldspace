/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef WINDOW_H
#define WINDOW_H

#include "../drivers/graphics.h"
#include <stdint.h> 

typedef struct {
    int x, y;                
    int width, height;       
    uint16_t border_color;   
    uint16_t fill_color;    
} Window;

void draw_window(Window *win); // Use const for making Window Features Read-only

#endif // WINDOW_H
