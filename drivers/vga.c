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
    // Unlock CRTC registers
    outb(VGA_CRTC_INDEX, 0x03);  // Disable screen
    outb(VGA_CRTC_DATA, 0x80);   // Protect registers

    // Set Mode 13h in the sequencer
    outb(VGA_SEQ_INDEX, 0x00);
    outb(VGA_SEQ_DATA, 0x03); // Unlock
    outb(VGA_SEQ_INDEX, 0x01);
    outb(VGA_SEQ_DATA, 0x01); // Mode 13h initialization

    // Set CRTC registers for 320x200 resolution
    outb(VGA_CRTC_INDEX, 0x11);
    outb(VGA_CRTC_DATA, 0xE3);

    // Graphics controller settings for mode 13h
    outb(VGA_GC_INDEX, 0x06);
    outb(VGA_GC_DATA, 0x05); // Select plane

    // Attribute controller settings
    outb(VGA_AC_INDEX, 0x20); // Set mode
    outb(VGA_AC_DATA, 0x00);

    clear_screen();
}
