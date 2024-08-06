#ifndef SYSCALL_HANDLERS_H
#define SYSCALL_HANDLERS_H

#include <stddef.h>

int sys_create_file(const char* name);
int sys_write_file(int file_index, const char* data, int size);
int sys_read_file(int file_index, char* buffer, int size);
int sys_delete_file(int file_index);

#endif // SYSCALL_HANDLERS_H
