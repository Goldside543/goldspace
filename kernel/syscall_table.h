/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef SYSCALL_TABLE_H
#define SYSCALL_TABLE_H

#include "syscall_numbers.h"

int sys_create_file(void* name, void* unused1, void* unused2, void* unused3);
int sys_write_file(void* file_index, void* data, void* size, void* unused);
int sys_read_file(void* file_index, void* buffer, void* size, void* unused);
int sys_delete_file(void* file_index, void* unused1, void* unused2, void* unused3);
int sys_execute_program(void* program_code, void* size, void* unused1, void* unused2);
int sys_yield(void* unused1, void* unused2, void* unused3, void* unused4);
int sys_exit(void* unused1, void* unused2, void* unused3, void* unused4);

// Declare the syscall table
extern int (*syscall_table[])(void*, void*, void*, void*);

void init_syscall_table();

#endif // SYSCALL_TABLE_H
