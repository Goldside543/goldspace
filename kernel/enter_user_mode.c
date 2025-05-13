// SPDX-License-Identifier: GPL-2.0-only
/*
 * kernel/enter_user_mode.c
 *
 * Basically, the first thing Goldspace does
 * when it enters user mode.
 *
 * Copyright (C) 2025 Goldside543
 *
 */

void puts(const char *str) {
    asm volatile (
        "mov $8, %%eax;"          // Load syscall number (8) into eax
        "mov %%ebx, 4(%%ebp);"    // Load the address of the string into 4(%%ebp)
        "mov %[str], %%ebx;"      // Move the string pointer to ebx (1st argument)
        "int $0x80;"              // Make the syscall
        :
        : [str] "r"(str)          // Input: 'str' (the string argument)
        : "%eax", "%ebx", "%ebp" // Clobbered registers
    );
}

int run_user_space() {
   puts("Hello, ring 3 and system calls!");

   for (;;) {
      asm volatile("nop");
   }
}
