#ifndef SYSCALL_DISPATCHER_H
#define SYSCALL_DISPATCHER_H

#include <stdint.h>

// Function to handle system calls
int syscall_dispatcher(uint32_t syscall_number, void* args);

#endif // SYSCALL_DISPATCHER_H
