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
        "pushf\n\t"                      // Push EFLAGS to stack (to save original flags)
        "cli\n\t"                        // Clear interrupts to prevent any interruptions during mode switch

        // Set up the V86 stack
        "pushl $0x23\n\t"                // SS (stack segment) for V86 mode (32-bit data segment)
        "pushl $0x2000\n\t"              // ESP (stack pointer) for V86 mode (start of the V86 stack)

        // Modify EFLAGS to set the VM flag (bit 17) and enable VM86 mode
        "popl %%eax\n\t"                 // Pop the original EFLAGS into EAX register
        "or $0x20000, %%eax\n\t"         // Set VM flag (bit 17) in EAX (enter VM86 mode)
        "pushl %%eax\n\t"                // Push modified EFLAGS back to the stack

        // Set up CS (code segment) and IP (instruction pointer) for VM86 mode
        "pushl $0x18\n\t"                // CS for V86 mode (index into V86 code segment descriptor)
        "pushl %0\n\t"                   // IP for V86 mode (function pointer to code_in_v86_mode)

        // Switch to VM86 mode and execute the code
        "iret\n\t"                        // IRET to enter VM86 mode and start executing in V86 mode

        :                                // No output operands
        : "r" (code_in_v86_mode)         // Input: function pointer to code to execute in VM86 mode
        : "eax"                          // Clobbered register: EAX
    );
}
