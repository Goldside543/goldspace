#ifndef SYSCALL_HANDLERS_H
#define SYSCALL_HANDLERS_H

#include <stddef.h>

// Define syscall numbers
#define SYSCALL_CREATE_FILE  0
#define SYSCALL_WRITE_FILE   1
#define SYSCALL_READ_FILE    2
#define SYSCALL_DELETE_FILE  3

int sys_create_file(const char* name);
int sys_write_file(int file_index, const char* data, int size);
int sys_read_file(int file_index, char* buffer, int size);
int sys_delete_file(int file_index);

#endif // SYSCALL_HANDLERS_H
