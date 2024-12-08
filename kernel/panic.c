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

void panic(const char *error_message) {
    // Print error message
    print("\n");
    print("KERNEL HAS ENCOUNTERED SEVERE,\n");
    print("IRRECOVERABLE ERROR!!! PLEASE REPORT THIS\n");
    print("ERROR TO A KERNEL MAINTAINER, AT\n");
    print("goldside000@outlook.com\n");
    print("\n");
    print("Cause of error: ");
    print(error_message);
    // Halt the CPU with an infinite loop
    while (1) {
	__asm__ volatile ("cli"); // Clear interrupts
	__asm__ volatile ("hlt"); // Halt CPU
        __asm__ volatile ("nop"); // Go into infinite loop if it somehow escapes the halt
    }
}
