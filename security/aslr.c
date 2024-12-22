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
static uint32_t random_seed = 0;

// Inline function to read the time-stamp counter (rdtsc)
static inline uint64_t read_tsc() {
    uint64_t tsc;
    asm volatile("rdtsc" : "=A" (tsc));  // Use TSC to get a value
    return tsc;
}

// Initialize the random seed based on the TSC value
void init_random_seed() {
    // Use the lower 32 bits of the TSC as the initial seed
    random_seed = (uint32_t)(read_tsc() & 0xFFFFFFFF);
}

// LCG-based random number generator
uint32_t rand32() {
    random_seed = random_seed * 1664525 + 1013904223;
    return random_seed;
}

uint32_t generate_random_address() {
    // Generate a random address within a specific range (2GB to 3GB)
    return (rand32() % (1024 * 1024 * 1024)) + 0x80000000;
}
