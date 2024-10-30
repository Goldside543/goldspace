// SPDX-License-Identifier: GPL-2.0-only
/*
 * kernel/panic.c
 *
 * Fallback kernel panic handling code, written in C.
 *
 * Copyright (C) 2024 Goldside543
 *
 */

void panic() {
    // Halt the CPU with an infinite loop
    while (1) {
        __asm__ volatile ("nop");
    }
}
