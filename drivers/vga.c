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
#define VGA_AC_WRITE   0x3C0
#define VGA_AC_READ    0x3C1
#define VGA_MISC_WRITE 0x3C2
#define VGA_MISC_READ  0x3CC
#define VGA_IS1_READ   0x3DA
#define VGA_MEMORY     0xA0000

// Mode 13h constants
#define MODE_13H_WIDTH  320
#define MODE_13H_HEIGHT 200
#define MODE_13H_DEPTH  256

void clear_vmemory() {
    // Clear text mode memory (0xB8000)
    uint16_t* text_mode = (uint16_t*)0xB8000;
    for (int i = 0; i < 80 * 25; i++) { // 80x25 characters
        text_mode[i] = 0x0700; // Black background, white text, space
    }

    // Clear graphics mode memory (0xA0000)
    uint8_t* graphics_mode = (uint8_t*)0xA0000;
    for (int i = 0; i < 320 * 200; i++) { // 320x200 pixels
        graphics_mode[i] = 0; // Black
    }
}

void clear_screen() {
    uint8_t* screen = (uint8_t*)VGA_MEMORY;
    for (int i = 0; i < (MODE_13H_WIDTH * MODE_13H_HEIGHT); i++) {
        screen[i] = 1; // Clear to color 1
    }
}

void set_mode_13h() {
    clear_vmemory();
    // Set miscellaneous output register for mode 13h
    outb(VGA_MISC_WRITE, 0x63); // Select graphics mode and clock
    
    // Sequencer settings
    outb(VGA_SEQ_INDEX, 0x00);  // Synchronous reset
    outb(VGA_SEQ_DATA, 0x01);
    outb(VGA_SEQ_INDEX, 0x01);  // Clocking mode
    outb(VGA_SEQ_DATA, 0x01);   // 8 dots/char, VGA compatible
    outb(VGA_SEQ_INDEX, 0x02);  // Map mask
    outb(VGA_SEQ_DATA, 0x0F);   // Enable all planes
    outb(VGA_SEQ_INDEX, 0x03);  // Character map select
    outb(VGA_SEQ_DATA, 0x00);
    outb(VGA_SEQ_INDEX, 0x04);  // Memory mode
    outb(VGA_SEQ_DATA, 0x0E);   // Enable chain-4 mode
    outb(VGA_SEQ_INDEX, 0x00);  // End reset
    outb(VGA_SEQ_DATA, 0x03);

    // Unlock CRTC registers
    outb(VGA_CRTC_INDEX, 0x11);
    outb(VGA_CRTC_DATA, inb(VGA_CRTC_DATA) & ~0x80);

    // Configure CRTC registers
    static const uint8_t crtc_values[] = {
        0x5F, 0x4F, 0x50, 0x82, 0x54, 0x80, 0xBF, 0x1F,
        0x00, 0x41, 0x0D, 0x0E, 0x00, 0x00, 0x00, 0x00,
        0x9C, 0x0E, 0x8F, 0x28, 0x40, 0x96, 0xB9, 0xA3,
        0xFF
    };
    for (int i = 0; i < 25; i++) {
        outb(VGA_CRTC_INDEX, i);
        outb(VGA_CRTC_DATA, crtc_values[i]);
    }

    // Graphics controller settings
    outb(VGA_GC_INDEX, 0x00);  // Set/reset
    outb(VGA_GC_DATA, 0x00);
    outb(VGA_GC_INDEX, 0x01);  // Enable set/reset
    outb(VGA_GC_DATA, 0x00);
    outb(VGA_GC_INDEX, 0x02);  // Color compare
    outb(VGA_GC_DATA, 0x00);
    outb(VGA_GC_INDEX, 0x03);  // Data rotate
    outb(VGA_GC_DATA, 0x00);
    outb(VGA_GC_INDEX, 0x04);  // Read map select
    outb(VGA_GC_DATA, 0x00);
    outb(VGA_GC_INDEX, 0x05);  // Graphics mode
    outb(VGA_GC_DATA, 0x40);   // Chain-4 mode
    outb(VGA_GC_INDEX, 0x06);  // Miscellaneous
    outb(VGA_GC_DATA, 0x05);
    outb(VGA_GC_INDEX, 0x08);  // Bit mask
    outb(VGA_GC_DATA, 0xFF);

    // Attribute controller settings
    for (int i = 0; i < 16; i++) {
        outb(VGA_AC_INDEX, i);
        outb(VGA_AC_WRITE, i);  // Set palette index
    }
    for (int i = 16; i < 32; i++) {
        outb(VGA_AC_INDEX, i);
        outb(VGA_AC_WRITE, i & 0x0F);  // Enable intensity bit
    }
    outb(VGA_AC_INDEX, 0x20);  // End attribute controller sequence

    // Clear the screen
    clear_screen();
}
