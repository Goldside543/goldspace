// SPDX-License-Identifier: GPL-2.0-only
/*
 * drivers/vga.c
 *
 * VGA driver.
 *
 * Copyright (C) 2024 Goldside543
 */

#include <stdint.h>
#include "../kernel/io.h"

// VGA ports
#define VGA_CRTC_INDEX 0x3D4
#define VGA_CRTC_DATA  0x3D5
#define VGA_SEQ_INDEX  0x3C4
#define VGA_SEQ_DATA   0x3C5
#define VGA_GC_INDEX   0x3CE
#define VGA_GC_DATA    0x3CF
#define VGA_AC_INDEX   0x3C0
#define VGA_AC_DATA    0x3C1
#define VGA_MISC_WRITE 0x3C2
#define VGA_MEMORY     0xA0000  // Correct segment address

// Mode 13h constants
#define MODE_13H_WIDTH  320
#define MODE_13H_HEIGHT 200
#define MODE_13H_DEPTH  256

void clear_screen() {
    uint8_t* screen = (uint8_t*)VGA_MEMORY;
    for (int i = 0; i < (MODE_13H_WIDTH * MODE_13H_HEIGHT); i++) {
        screen[i] = 0; // Clear to color 0 (black)
    }
}

void set_mode_13h() {
    // Set miscellaneous output register for mode 13h
    outb(VGA_MISC_WRITE, 0x63);

    // Sequencer settings
    outb(VGA_SEQ_INDEX, 0x00);  // Synchronous reset
    outb(VGA_SEQ_DATA, 0x03);   // End reset
    outb(VGA_SEQ_INDEX, 0x01);  // Clocking mode
    outb(VGA_SEQ_DATA, 0x01);
    outb(VGA_SEQ_INDEX, 0x02);  // Map mask
    outb(VGA_SEQ_DATA, 0x0F);   // Enable all planes
    outb(VGA_SEQ_INDEX, 0x04);  // Memory mode
    outb(VGA_SEQ_DATA, 0x06);   // Enable plane A, sequential access

    // Unlock CRTC registers
    outb(VGA_CRTC_INDEX, 0x11);
    outb(VGA_CRTC_DATA, 0x00);

    // CRTC settings
    static const uint8_t crtc_values[] = {
        0x5F, 0x4F, 0x50, 0x82, 0x55, 0x81, 0xBF, 0x1F,
        0x00, 0x4F, 0x0D, 0x0E, 0x00, 0x00, 0x00, 0x00,
        0x9C, 0x0E, 0x8F, 0x28, 0x40, 0x96, 0xB9, 0xA3,
        0xFF
    };
    for (int i = 0; i < 25; i++) {
        outb(VGA_CRTC_INDEX, i);
        outb(VGA_CRTC_DATA, crtc_values[i]);
    }

    // Graphics controller settings
    outb(VGA_GC_INDEX, 0x05);   // Mode register
    outb(VGA_GC_DATA, 0x40);    // 256-color mode
    outb(VGA_GC_INDEX, 0x06);   // Miscellaneous register
    outb(VGA_GC_DATA, 0x05);

    // Attribute controller settings
    for (int i = 0; i < 16; i++) {
        outb(VGA_AC_INDEX, i);
        outb(VGA_AC_DATA, i);
    }
    outb(VGA_AC_INDEX, 0x20);   // End attribute mode

    // Clear the screen
    clear_screen();
}
