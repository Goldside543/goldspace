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

int run_user_space() {
   while(1) {
      asm volatile("nop"); // do nothing
   }
}
