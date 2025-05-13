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
        "push %[str];"         // Push the string argument onto the stack
        "mov $8, %%eax;"       // Load syscall number (8) into eax
        "int $0x80;"           // Trigger syscall interrupt
        "add $4, %%esp;"       // Clean up the stack by removing the pushed argument
        :
        : [str] "r"(str)       // Input: pass the string pointer to the assembly function
        : "%eax", "%esp"       // Clobbered registers
    );
}

int run_user_space() {
   puts("Hello, ring 3 and system calls!");

   for (;;) {
      asm volatile("nop");
   }
}
