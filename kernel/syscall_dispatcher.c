// SPDX-License-Identifier: GPL-2.0-only
/*
 * kernel/syscall_dispatcher.c
 * 
 * System call handler.
 *
 * Copyright (C) 2024 Goldside543
 *
 */

#include "syscall_table.h"
#include "syscall_numbers.h"
#include "print.h"

#define SYSCALL_TABLE_SIZE 9

int syscall_handler(int syscall_number, void* arg1, void* arg2, void* arg3, void* arg4) {
    // Check if syscall_number is within valid range
    if (syscall_number < 0 || syscall_number >= SYSCALL_TABLE_SIZE)
        return -1;

    // Get the function pointer from the syscall_table
    int (*handler)(void*, void*, void*, void*) = syscall_table[syscall_number];

    // Call the syscall handler with the provided arguments
    return handler(arg1, arg2, arg3, arg4);
}
