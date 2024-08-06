#include "syscall_table.h"
#include "syscall_numbers.h"

int syscall_handler(int syscall_number, void* arg1, void* arg2, void* arg3, void* arg4) {
    // Check if syscall_number is within valid range
    if (syscall_number < 0 || syscall_number >= (sizeof(syscall_table[syscall_number]) / sizeof(syscall_table[0]))) {
        return -1;  // Invalid syscall number
    }

    // Get the function pointer from the syscall_table
    int (*handler)(void*, void*, void*, void*) = syscall_table[syscall_number];

    // Call the syscall handler with the provided arguments
    return handler(arg1, arg2, arg3, arg4);
}
