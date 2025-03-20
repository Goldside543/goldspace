/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef SYSCALL_TABLE_H
#define SYSCALL_TABLE_H

#include "syscall_numbers.h"

int sys_execve(void* path, void* argv, void* unused1, void* unused2);
int sys_yield(void* unused1, void* unused2, void* unused3, void* unused4);
int sys_exit(void* unused1, void* unused2, void* unused3, void* unused4);

// Declare the syscall table
extern int (*syscall_table[])(void*, void*, void*, void*);

void init_syscall_table();

#endif // SYSCALL_TABLE_H
