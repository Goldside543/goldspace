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

void enter_vm86_mode(void (*code_in_v86_mode)()) {
    asm volatile (
        // Save the original EFLAGS (flags register) to the stack
        "pushf\n\t"                  // Save original EFLAGS to stack
        "cli\n\t"                    // Clear interrupts to prevent interruptions

        // Prepare stack for V86 mode
        "pushl $0x23\n\t"            // SS (stack segment) for V86 mode (32-bit data segment)
        "pushl $0x2000\n\t"          // ESP (stack pointer) for V86 mode (start of the V86 stack)

        // Set up EFLAGS with VM flag set (enter VM86 mode)
        "pushf\n\t"                  // Push the current EFLAGS again to modify
        "popl %%eax\n\t"             // Pop into EAX to modify the flags
        "or $0x20000, %%eax\n\t"     // Set the VM flag (bit 17) in EFLAGS to enter VM86 mode
        "pushl %%eax\n\t"            // Push modified EFLAGS back onto the stack

        // Set up CS (code segment) and IP (instruction pointer) for VM86 mode
        "pushl $0x18\n\t"            // CS for V86 mode (index into V86 code segment descriptor)
        "pushl %0\n\t"               // IP (instruction pointer) for the code to execute in V86 mode (function pointer)

        // Perform IRET to switch into V86 mode and execute the code
        "iret\n\t"

        :
        : "r" (code_in_v86_mode)     // Pass the function pointer for the code to execute in V86 mode
        : "eax"
    );
}
