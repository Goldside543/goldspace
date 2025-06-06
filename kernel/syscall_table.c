// SPDX-License-Identifier: GPL-2.0-only
/*
 * kernel/syscall_table.c
 *
 * System call table definition.
 *
 * Copyright (C) 2024-2025 Goldside543
 *
 */

#include "syscall_table.h"
#include "../fs/vfs/vfs.h"

// Define the syscall table
int (*syscall_table[])(void*, void*, void*, void*) = {
    [SYS_OPEN]     = (int (*)(void*, void*, void*, void*))vfs_open,
    [SYS_WRITE]     = (int (*)(void*, void*, void*, void*))vfs_write,
    [SYS_READ]      = (int (*)(void*, void*, void*, void*))vfs_read,
    [SYS_CLOSE]      = (int (*)(void*, void*, void*, void*))vfs_close,
    [SYS_EXECV] = (int (*)(void*, void*, void*, void*))sys_execv,
    [SYS_YIELD]          = (int (*)(void*, void*, void*, void*))sys_yield,
    [SYS_EXIT]          = (int (*)(void*, void*, void*, void*))sys_exit,
    [SYS_STAT]          = (int (*)(void*, void*, void*, void*))vfs_stat,
    [SYS_TESTPUTS]      = (int (*)(void*, void*, void*, void*))sys_testputs,
};
