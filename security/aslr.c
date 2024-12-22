// SPDX-License-Identifier: GPL-2.0-only
/*
 * security/aslr.c
 *
 * Address Space Layout Randomization. Where's
 * the base address of that process? Haha, you
 * don't get to know!
 *
 * Copyright (C) 2024 Goldside543
 *
 */

#include <stdint.h>
#include <stddef.h>

// Simple pseudo-random number generator (LCG method)
static uint32_t random_seed = 0x12345678;

uint32_t rand32() {
    random_seed = random_seed * 1664525 + 1013904223;
    return random_seed;
}

uint32_t generate_random_address() {
    // Return a random address within a certain range, e.g., 2GB to 3GB
    return (rand32() % (1024 * 1024 * 1024)) + 0x80000000;
}
