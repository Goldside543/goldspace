// SPDX-License-Identifier: GPL-2.0-only
/*
 * kernel/panic.c
 *
 * Fallback kernel panic handling code, written in C.
 *
 * Copyright (C) 2024 Goldside543
 *
 */

#include "print.h"

void panic() {
    // Print error message
    print("\n");
    print("ERROR!!! KERNEL HAS ENCOUNTERED SEVERE,\n");
    print("IRRECOVERABLE ERROR!!! PLEASE REPORT THIS\n");
    print("ERROR TO EITHER AN OS MAINTAINER OR KERNEL\n");
    print("MAINTAINER, THE LATTER AT\n");
    print("goldside000@outlook.com");
    // Halt the CPU with an infinite loop
    while (1) {
        __asm__ volatile ("nop");
    }
}
