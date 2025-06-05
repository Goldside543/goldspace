// SPDX-License-Identifier: GPL-2.0-only
/*
 * kernel/interrupt.c
 *
 * Interrupt handling.
 *
 * Copyright (C) 2024 Goldside543
 *
 */

#include <stdint.h>
#include "syscall_dispatcher.h"

// Interrupt handler for the software interrupt
void software_interrupt_handler() {

    // Get the system call number from EAX
    int syscall_number;
    asm volatile ("movl %%eax, %0" : "=r"(syscall_number));

    // Retrieve arguments from the stack
    void* arg1;
    void* arg2;
    void* arg3;
    void* arg4;

    asm volatile (
        "movl %%ebx, %0\n"   // Get the first argument (arg1)
        "movl %%ecx, %1\n"   // Get the second argument (arg2)
        "movl %%edx, %2\n"  // Get the third argument (arg3)
        "movl %%esi, %3"    // Get the fourth argument (arg4)
        : "=r"(arg1), "=r"(arg2), "=r"(arg3), "=r"(arg4)
        : /* no input */
        : /* no clobber */
    );

    int result = syscall_handler(syscall_number, arg1, arg2, arg3, arg4);

    asm volatile ("movl %0, %%eax" : : "r"(result));
}
