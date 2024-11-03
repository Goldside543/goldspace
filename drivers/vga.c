// SPDX-License-Identifier: GPL-2.0-only
/*
 * drivers/vga.c
 *
 * VGA driver.
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
#define VGA_MISC_WRITE 0x3C2
#define VGA_MEMORY     0xA0000  // Full address in protected mode

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

// Function to set VGA mode 13h (320x200, 256 colors)
void set_mode_13h() {
    // Set the miscellaneous output register for 256-color mode
    outb(VGA_MISC_WRITE, 0x63);

    // Set the sequencer registers
    outb(VGA_SEQ_INDEX, 0x00);  // Reset sequencer
    outb(VGA_SEQ_DATA, 0x01);   // Set clocking mode
    outb(VGA_SEQ_INDEX, 0x01);  // Clocking mode
    outb(VGA_SEQ_DATA, 0x01);   // Select 25 MHz
    outb(VGA_SEQ_INDEX, 0x02);  // Map mask
    outb(VGA_SEQ_DATA, 0x0F);   // Enable all planes
    outb(VGA_SEQ_INDEX, 0x04);  // Memory mode
    outb(VGA_SEQ_DATA, 0x02);   // Enable 256-color mode

    // Unlock CRTC registers
    outb(VGA_CRTC_INDEX, 0x11); // Unlock vertical retrace end
    outb(VGA_CRTC_DATA, 0x00);  // Set vertical retrace end

    // Set CRTC registers for 320x200 resolution
    outb(VGA_CRTC_INDEX, 0x00); // Horizontal total
    outb(VGA_CRTC_DATA, 0x0A);  // Value for horizontal total
    outb(VGA_CRTC_INDEX, 0x01); // Vertical total
    outb(VGA_CRTC_DATA, 0xC8);  // Value for vertical total
    outb(VGA_CRTC_INDEX, 0x13); // Underline location
    outb(VGA_CRTC_DATA, 0x20);  // Mode 13h value

    // Graphics controller settings for mode 13h
    outb(VGA_GC_INDEX, 0x05);   // Set graphics mode
    outb(VGA_GC_DATA, 0x40);    // Enable 256-color mode
    outb(VGA_GC_INDEX, 0x06);   // Miscellaneous graphics
    outb(VGA_GC_DATA, 0x05);    // Set to 256-color mode

    // Initialize the color palette
    for (int i = 0; i < 256; i++) {
        outb(VGA_AC_INDEX, i);    // Select color index
        outb(VGA_AC_DATA, i);     // Load color (simple grayscale for test)
    }
    outb(VGA_AC_INDEX, 0x20);   // End attribute controller setup

    // Clear screen to black
    clear_screen();
}
