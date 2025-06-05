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
void software_interrupt_handler(int syscall_number, void *arg1, void *arg2, void *arg3, void *arg4) {
    int result = syscall_handler(syscall_number, arg1, arg2, arg3, arg4);

    asm volatile ("movl %0, %%eax" : : "r"(result));
}
