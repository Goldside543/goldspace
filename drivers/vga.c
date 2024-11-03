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
#define VGA_MEMORY     0xA000

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
    // Set miscellaneous output register for 256-color mode
    outb(VGA_MISC_WRITE, 0x63);

    // Set sequencer registers
    outb(VGA_SEQ_INDEX, 0x00);  // Reset sequencer
    outb(VGA_SEQ_DATA, 0x03);   
    outb(VGA_SEQ_INDEX, 0x01);  // Clocking mode
    outb(VGA_SEQ_DATA, 0x01);   
    outb(VGA_SEQ_INDEX, 0x02);  // Map mask
    outb(VGA_SEQ_DATA, 0x0F);   // Enable all planes
    outb(VGA_SEQ_INDEX, 0x04);  // Memory mode
    outb(VGA_SEQ_DATA, 0x0E);   // Enable plane A, color 256-color mode

    // Unlock CRTC registers
    outb(VGA_CRTC_INDEX, 0x03);  // Unlock CRTC
    outb(VGA_CRTC_DATA, 0x80);   

    // Set CRTC registers for 320x200 resolution
    outb(VGA_CRTC_INDEX, 0x11);  // Unlock vertical retrace end
    outb(VGA_CRTC_DATA, 0x0C);   
    outb(VGA_CRTC_INDEX, 0x13);  // Underline location
    outb(VGA_CRTC_DATA, 0x20);   // Mode 13h value

    // Graphics controller settings for mode 13h
    outb(VGA_GC_INDEX, 0x05);    // Graphics mode register
    outb(VGA_GC_DATA, 0x40);     // 256-color mode
    outb(VGA_GC_INDEX, 0x06);    // Miscellaneous graphics
    outb(VGA_GC_DATA, 0x05);     

    // Attribute controller settings
    for (int i = 0; i < 16; i++) {
        outb(VGA_AC_INDEX, i);
        outb(VGA_AC_DATA, i); // Load default color palette
    }
    outb(VGA_AC_INDEX, 0x20);  // End attribute controller setup

    // Clear screen to black
    clear_screen();
}
