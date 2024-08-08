// SPDX-License-Identifier: GPL-2.0-only
/*
 * kernel/cpudelay.c
 *
 * CPU cycle wasting function.
 *
 * Copyright (C) 2024 Goldside543
 *
 */

// Function to create a delay in CPU cycles
void cpu_delay(unsigned int cycles) {
    volatile unsigned int count;
    for (count = 0; count < cycles; count++) {
        // Empty loop to waste CPU cycles
    }
}
