// SPDX-License-Identifier: GPL-2.0-only
/*
 * kernel/syscall_table.c
 *
 * System call table definition.
 *
 * Copyright (C) 2024 Goldside543
 *
 */

#include "syscall_table.h"

// Define the syscall table
int (*syscall_table[])(void*, void*, void*, void*) = {
    [SYS_CREATE_FILE]    = (int (*)(void*, void*, void*, void*))sys_create_file,
    [SYS_WRITE_FILE]     = (int (*)(void*, void*, void*, void*))sys_write_file,
    [SYS_READ_FILE]      = (int (*)(void*, void*, void*, void*))sys_read_file,
    [SYS_DELETE_FILE]    = (int (*)(void*, void*, void*, void*))sys_delete_file,
    [SYS_EXECUTE_PROGRAM] = (int (*)(void*, void*, void*, void*))sys_execute_program,
    [SYS_YIELD]          = (int (*)(void*, void*, void*, void*))sys_yield,
    [SYS_EXIT]          = (int (*)(void*, void*, void*, void*))sys_exit,
};
