// SPDX-License-Identifier: GPL-2.0-only
/*
 * drivers/audio.c
 *
 * Audio driver.
 *
 * Copyright (C) 2024 Goldside543
 *
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
#define VGA_MEMORY     0xA0000

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

// Function to set VGA mode 13h
void set_mode_13h() {
    // Set the VGA to mode 13h (320x200, 256 colors)
    
    // Set the sequence registers
    outb(VGA_SEQ_INDEX, 0x01); // Unlock
    outb(VGA_SEQ_DATA, 0x01);  // Set to mode 13h
    outb(VGA_SEQ_INDEX, 0x00);
    outb(VGA_SEQ_DATA, 0x03); // Set to 256 colors

    // Set the CRTC registers
    outb(VGA_CRTC_INDEX, 0x11); // Set vertical total
    outb(VGA_CRTC_DATA, 0xD3);  // 200 lines

    // Set the graphics controller registers
    outb(VGA_GC_INDEX, 0x05); // Set to graphics mode
    outb(VGA_GC_DATA, 0x00);  // Disable all features

    // Set the attribute controller
    outb(VGA_AC_INDEX, 0x20); // Set mode
    outb(VGA_AC_DATA, 0x00);  // Enable color palette

    // Clear the screen
    clear_screen();
}
