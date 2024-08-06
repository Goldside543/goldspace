#ifndef SYSCALL_DISPATCHER_H
#define SYSCALL_DISPATCHER_H

#include "syscall_numbers.h"
#include <stdint.h>

// Function prototype for the syscall handler
int syscall_handler(int syscall_number, void* arg1, void* arg2, void* arg3, void* arg4);

#endif // SYSCALL_DISPATCHER_H
