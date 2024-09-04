// SPDX-License-Identifier: GPL-2.0-only
/*
 * arch/x86_64/print64.c
 *
 * 64-bit version of the print function.
 *
 * Copyright (C) 2024 Goldside543
 *
 */

#include "../../kernel/print.h"
#include "print64.h"
#include "../../kernel/core.c"

#define VGA_ADDRESS 0xB8000 // 0xB8000 can be a Problem
#define VGA_ENTRY(c, color) ((color << 8) | (c & 0xFF))

static uint16_t *const VGA_BUFFER = (uint16_t *)VGA_ADDRESS;
static uint8_t cursor_x = 0;
static uint8_t cursor_y = 0;
static uint8_t text_color = 0x0F;  // Default white text on black background

static void move_cursor(void) {
    uint16_t pos = cursor_y * VGA_WIDTH + cursor_x;
    // The cursor position registers are not being used here, just updating the VGA buffer
}

void print(const char *str) {
    while (*str != '\0') {
        switch (*str) {
            case '\n':
                cursor_x = 0;
                cursor_y++;
                break;
            case '\b':  // Handle backspace
                if (cursor_x > 0) {
                    cursor_x--;
                    VGA_BUFFER[cursor_y * VGA_WIDTH + cursor_x] = VGA_ENTRY(' ', text_color);
                }
                break;
            default:
                // Write character to VGA buffer at current cursor position
                VGA_BUFFER[cursor_y * VGA_WIDTH + cursor_x] = VGA_ENTRY(*str, text_color);
                cursor_x++;
                break;
        }

        // Check if we need to wrap to the next line
        if (cursor_x >= VGA_WIDTH) {
            cursor_x = 0;
            cursor_y++;
        }

        // Check if we need to scroll
        if (cursor_y >= VGA_HEIGHT) {
            // Scroll up by copying each line up one row
            for (int i = 0; i < (VGA_HEIGHT - 1) * VGA_WIDTH; i++) {
                VGA_BUFFER[i] = VGA_BUFFER[i + VGA_WIDTH];
            }
            // Clear the last line
            for (int i = (VGA_HEIGHT - 1) * VGA_WIDTH; i < VGA_HEIGHT * VGA_WIDTH; i++) {
                VGA_BUFFER[i] = VGA_ENTRY(' ', text_color);
            }
            cursor_y = VGA_HEIGHT - 1;
        }

        str++;
    }
    move_cursor();
}
