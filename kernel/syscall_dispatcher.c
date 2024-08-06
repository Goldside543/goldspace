#include "syscall_numbers.h"
#include "syscall_table.h"

int syscall_handler(int syscall_number, void* args) {
    if (syscall_number < 0 || syscall_number >= sizeof(syscall_table) / sizeof(syscall_table[0])) {
        return -1; // Invalid syscall number
    }
    
    int (*handler)(void*) = syscall_table[syscall_number];
    if (handler == NULL) {
        return -1; // No handler found
    }
    
    return handler(args);
}
