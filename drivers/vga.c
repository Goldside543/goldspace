// SPDX-License-Identifier: GPL-2.0-only
/*
 * drivers/vga.c
 *
 * VGA driver.
 *
 * Copyright (C) 2024-2025 Goldside543
 */

// vga_mode13.c
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

static void clear_text_mode_memory(void) {
    uint16_t* text_mode = (uint16_t*)0xB8000;
    for (int i = 0; i < 80 * 25; i++) {
        text_mode[i] = 0x0700; // space, white on black
    }
}

static void clear_graphics_memory(void) {
    uint8_t* graphics_mode = (uint8_t*)VGA_MEMORY;
    for (int i = 0; i < MODE_13H_WIDTH * MODE_13H_HEIGHT; i++) {
        graphics_mode[i] = 0; // black
    }
}

void clear_vmemory() {
    clear_text_mode_memory();
    clear_graphics_memory();
}

void clear_screen() {
    uint8_t* screen = (uint8_t*)VGA_MEMORY;
    for (int i = 0; i < (MODE_13H_WIDTH * MODE_13H_HEIGHT); i++) {
        screen[i] = 1; // fill with palette color 1
    }
}

/*
 * Set VGA mode 13h (320x200x256).
 *
 * Key fixes compared to the original:
 *  - Use the proper sequencer synchronous reset sequence (start reset, program, end reset).
 *  - For Attribute Controller writes: read input status 1 (0x3DA) before every write to 0x3C0
 *    to flip the attribute controller's internal index latch. Failing to do so makes writes
 *    ignored or misrouted.
 *  - Unlock CRTC register 0x11 correctly (read-modify-write on the data port after indexing).
 *  - Provide the correct typical GC and attribute mode registers.
 *
 * Warning: this code assumes you are running on a color VGA (ports 0x3D4/0x3D5). If you're on
 * a monochrome adapter you'd need to use 0x3B4/0x3B5 for CRTC.
 */
void set_mode_13h() {
    clear_vmemory();

    // 1) Misc output: select clock and enable graphics
    outb(VGA_MISC_WRITE, 0x63);

    // 2) Sequencer: synchronous reset
    // Enter reset
    outb(VGA_SEQ_INDEX, 0x00);
    outb(VGA_SEQ_DATA, 0x01);

    // Program other sequencer registers
    static const uint8_t seq_values[5] = {
        0x03, // Index 0 (will be set to 0x03 at the end of reset) - safe default
        0x01, // Clocking mode: 8 dots/char, VGA compatible clocking
        0x0F, // Map mask: enable all 4 plane writes
        0x00, // Character map select
        0x0E  // Memory mode: chain-4 (planar->linear) and enable extended memory
    };
    for (int i = 1; i < 5; ++i) {
        outb(VGA_SEQ_INDEX, i);
        outb(VGA_SEQ_DATA, seq_values[i]);
    }

    // End reset: write index 0 with 0x03 (normal operation)
    outb(VGA_SEQ_INDEX, 0x00);
    outb(VGA_SEQ_DATA, 0x03);

    // 3) Unlock CRTC registers (clear bit 7 of register 0x11)
    outb(VGA_CRTC_INDEX, 0x11);
    uint8_t crtc11 = inb(VGA_CRTC_DATA);
    crtc11 &= ~0x80; // clear bit 7 (protect bit)
    outb(VGA_CRTC_DATA, crtc11);

    // 4) Program CRTC registers (standard values for 320x200x256)
    static const uint8_t crtc_values[25] = {
        0x5F, 0x4F, 0x50, 0x82, 0x54, 0x80, 0xBF, 0x1F,
        0x00, 0x41, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x9C, 0x0E, 0x8F, 0x28, 0x40, 0x96, 0xB9, 0xA3,
        0xFF
    };
    for (int i = 0; i < 25; ++i) {
        outb(VGA_CRTC_INDEX, i);
        outb(VGA_CRTC_DATA, crtc_values[i]);
    }

    // 5) Graphics controller registers
    static const uint8_t gc_values[9] = {
        0x00, // Set/Reset
        0x00, // Enable set/reset
        0x00, // Color compare
        0x00, // Data rotate
        0x00, // Read map select
        0x40, // Graphics mode: Chain-4 / read/write
        0x05, // Miscellaneous: enable odd/even? typical value for mode 13h
        0x0F, // Color don't-care (plane mask) (index 7 usually unused in some docs)
        0xFF  // Bit mask (index 8)
    };
    // indexes: 0..8 but GC index 7 isn't always used the same; we keep safe writes
    for (int i = 0; i <= 8; ++i) {
        outb(VGA_GC_INDEX, i);
        outb(VGA_GC_DATA, gc_values[i]);
    }

    // 6) Attribute controller (must read input status 1 before each write)
    // Palette 0..15
    for (int i = 0; i < 16; ++i) {
        (void)inb(VGA_IS1_READ);       // flip the index latch
        outb(VGA_AC_INDEX, i);         // put index into AC
        outb(VGA_AC_WRITE, (uint8_t)i); // data: palette entry = i
    }

    // Attribute controller: mode control, overscan, color plane enable, horizontal panning, color select
    // Values chosen to match standard mode 13h setup:
    (void)inb(VGA_IS1_READ);
    outb(VGA_AC_INDEX, 0x10); outb(VGA_AC_WRITE, 0x41); // Attribute Mode Control: 0x41 typical
    (void)inb(VGA_IS1_READ);
    outb(VGA_AC_INDEX, 0x11); outb(VGA_AC_WRITE, 0x00); // Overscan / border color
    (void)inb(VGA_IS1_READ);
    outb(VGA_AC_INDEX, 0x12); outb(VGA_AC_WRITE, 0x0F); // Color Plane Enable (all planes)
    (void)inb(VGA_IS1_READ);
    outb(VGA_AC_INDEX, 0x13); outb(VGA_AC_WRITE, 0x00); // Horizontal Panning
    (void)inb(VGA_IS1_READ);
    outb(VGA_AC_INDEX, 0x14); outb(VGA_AC_WRITE, 0x00); // Color Select

    // Signal end of attribute controller programming by writing index 0x20
    (void)inb(VGA_IS1_READ);
    outb(VGA_AC_INDEX, 0x20);

    // 7) Clear the screen to a known color
    clear_screen();
}
