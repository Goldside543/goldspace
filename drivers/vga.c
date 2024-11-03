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

// Function to set VGA mode 13h
void set_mode_13h() {
    // Miscellaneous output register
    outb(VGA_MISC_WRITE, 0x63);

    // Unlock CRTC registers
    outb(VGA_CRTC_INDEX, 0x03);  // Disable screen
    outb(VGA_CRTC_DATA, 0x80);   // Protect registers

    // Set sequencer registers for mode 13h
    outb(VGA_SEQ_INDEX, 0x00); // Reset sequencer
    outb(VGA_SEQ_DATA, 0x03);  
    outb(VGA_SEQ_INDEX, 0x01);
    outb(VGA_SEQ_DATA, 0x01);  // Clocking mode for 320x200
    outb(VGA_SEQ_INDEX, 0x04); // Memory mode
    outb(VGA_SEQ_DATA, 0x0E);  // Enable plane A

    // Set CRTC registers for 320x200 resolution
    outb(VGA_CRTC_INDEX, 0x11);  // Vertical retrace end
    outb(VGA_CRTC_DATA, 0x0C);   // Clear bit 7 for mode 13h
    outb(VGA_CRTC_INDEX, 0x13);  // Underline location register
    outb(VGA_CRTC_DATA, 0x20);   // Set to 0x20 for mode 13h

    // Graphics controller settings for mode 13h
    outb(VGA_GC_INDEX, 0x05); // Graphics mode
    outb(VGA_GC_DATA, 0x40);  // Shift 256-color mode
    outb(VGA_GC_INDEX, 0x06); // Miscellaneous graphics register
    outb(VGA_GC_DATA, 0x05);  // Graphics mode settings

    // Attribute controller settings
    for (int i = 0; i < 16; i++) {
        outb(VGA_AC_INDEX, i);
        outb(VGA_AC_DATA, i);
    }
    outb(VGA_AC_INDEX, 0x20);  // End of attribute controller sequence

    clear_screen();
}
