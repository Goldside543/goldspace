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
#include <stdbool.h>

// Get hardware random number
static inline bool rdrand32(uint32_t *val) {
    unsigned char success;
    asm volatile("rdrand %0; setc %1" : "=r" (*val), "=qm" (success));
    return success;
}

uint32_t rand32() {
    uint32_t val;
    if (rdrand32(&val)) {
        return val;  // Use RDRAND if available
    }

    // Fallback: Use a hashed TSC if RDRAND fails (rare but possible)
    uint32_t low, high;
    asm volatile("rdtsc" : "=a" (low), "=d" (high));  // Get TSC value
    val = (high << 16) | (low & 0xFFFF);  // Mix bits for better entropy

    // Simple hashing to mix bits
    val ^= val >> 16;
    val *= 0x85ebca6b;
    val ^= val >> 13;
    val *= 0xc2b2ae35;
    val ^= val >> 16;

    return val;
}

// Generate a randomized address for ASLR
uint32_t generate_random_address() {
    return (rand32() & 0x3FFFFFFF) + 0x80000000;  // Keeps it within 2GB-3GB
}
