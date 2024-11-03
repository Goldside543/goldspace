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
    // Ensure that the VGA is ready for mode changes
    outb(VGA_MISC_WRITE, 0x63); // Set the Miscellaneous Output Register

    // Set sequencer registers
    outb(VGA_SEQ_INDEX, 0x00);  // Reset sequencer
    outb(VGA_SEQ_DATA, 0x03);
    outb(VGA_SEQ_INDEX, 0x01);  // Clocking mode
    outb(VGA_SEQ_DATA, 0x01);
    outb(VGA_SEQ_INDEX, 0x02);  // Map mask
    outb(VGA_SEQ_DATA, 0x0F);   // Enable all planes
    outb(VGA_SEQ_INDEX, 0x04);  // Memory mode
    outb(VGA_SEQ_DATA, 0x0E);   // Enable plane A, 256-color mode

    // Unlock CRTC registers (requires bit 0 of MISC to be set)
    outb(VGA_CRTC_INDEX, 0x11); // Unlock vertical retrace end
    outb(VGA_CRTC_DATA, 0x00);  // Reset CRTC register

    // Set CRTC registers for 320x200 resolution
    outb(VGA_CRTC_INDEX, 0x00); // Reset counter
    outb(VGA_CRTC_DATA, 0x5F);  // Total vertical lines (0x5F = 95 lines + 1 for retrace)

    outb(VGA_CRTC_INDEX, 0x01); // Vertical display enable
    outb(VGA_CRTC_DATA, 0x00);  // Display end (0x00 for 320x200)

    outb(VGA_CRTC_INDEX, 0x02); // Start address high
    outb(VGA_CRTC_DATA, 0x00);  // Set start address

    outb(VGA_CRTC_INDEX, 0x03); // Reset counter
    outb(VGA_CRTC_DATA, 0x00);  // Misc value

    outb(VGA_CRTC_INDEX, 0x04); // Vertical compare
    outb(VGA_CRTC_DATA, 0x00);  // Reset

    outb(VGA_CRTC_INDEX, 0x05); // Mode control
    outb(VGA_CRTC_DATA, 0x00);  // Set line width (0 = 320 pixels)

    outb(VGA_CRTC_INDEX, 0x06); // Offset
    outb(VGA_CRTC_DATA, 0x00);  // Reset

    outb(VGA_CRTC_INDEX, 0x07); // More CRTC settings
    outb(VGA_CRTC_DATA, 0x0F);  // Set more values

    outb(VGA_CRTC_INDEX, 0x08); // More CRTC settings
    outb(VGA_CRTC_DATA, 0x00);  // Set more values

    // Graphics controller settings for mode 13h
    outb(VGA_GC_INDEX, 0x05);   // Set Graphics mode
    outb(VGA_GC_DATA, 0x40);    // Enable 256-color mode
    outb(VGA_GC_INDEX, 0x06);   // Set Miscellaneous graphics
    outb(VGA_GC_DATA, 0x05);    

    // Initialize the DAC
    outb(0x3C6, 0xFF);           // Set DAC mask to allow all colors

    // Attribute controller settings
    outb(VGA_AC_INDEX, 0x00);    // Start at the first palette index
    for (int i = 0; i < 256; i++) {
        outb(VGA_AC_DATA, i);     // Load default color palette
    }
    outb(VGA_AC_INDEX, 0x20);     // End attribute controller setup

    // Clear screen to black
    clear_screen();
}
