#ifndef SYSCALL_TABLE_H
#define SYSCALL_TABLE_H

#include "syscall_numbers.h"
#include "../fs/fs_syscalls.h"

// Define the syscall table
extern int (*syscall_table[])(void);

// Function to initialize the syscall table
void init_syscall_table();

#endif // SYSCALL_TABLE_H
