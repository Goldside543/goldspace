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
        "mov %[str], %%ebx;"    // Push the string argument onto the stack
        "mov $8, %%eax;"       // Load syscall number (8) into eax
        "int $0x80;"           // Trigger syscall interrupt
        :
        : [str] "r"(str)       // Input: pass the string pointer to the assembly function
        : "%eax"               // Clobbered registers
    );
}

int run_user_space() {
   puts("Hello, ring 3 and system calls!");

   for (;;) {
      asm volatile("nop");
   }
}
