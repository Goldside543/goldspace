// SPDX-License-Identifier: GPL-2.0-only
/*
 * kernel/v86.c
 * 
 * Switches the CPU to Virtual 8086 mode
 * to execute 16-bit code.
 *
 * Copyright (C) 2024 Goldside543
 *
 */

void enter_vga_graphics_mode() {
    asm volatile (
        "pushfl\n\t"                // Save original EFLAGS to stack
        "cli\n\t"                   // Clear interrupts

        // Prepare stack for V86 mode
        "pushl $0x23\n\t"           // SS (stack segment) for V86 mode
        "pushl $0x2000\n\t"         // ESP (stack pointer) for V86 mode

        // Set up EFLAGS with VM flag set
        "pushfl\n\t"                // Push original EFLAGS again
        "popl %%eax\n\t"            // Pop into EAX to modify
        "or $0x20000, %%eax\n\t"    // Set VM (bit 17) in EFLAGS (for V86)
        "pushl %%eax\n\t"           // Push modified EFLAGS

        // Set up CS and IP for V86 mode
        "pushl $0x8\n\t"            // CS for V86 mode
        "pushl $v86_code\n\t"       // IP (instruction pointer) in V86 mode

        "iret\n"                    // Enter V86 mode with IRET

        // Code to run in V86 mode
        "v86_code:\n\t"
        "movb $0x03, %%al\n\t"      // Example V86-mode code
        "out %%al, $0xF8\n\t"      // Example I/O instruction

        // Exit V86 mode (jump back to 32-bit protected mode)
        "cli\n\t"                   // Disable interrupts again in 32-bit mode
        "popfl\n\t"                 // Restore original EFLAGS
        :
        :
        : "eax"
    );
}
