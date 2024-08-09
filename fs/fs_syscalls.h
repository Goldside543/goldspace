/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef FS_SYSCALLS_H
#define FS_SYSCALLS_H

#include <stddef.h>

int sys_create_file(const char* name);
int sys_write_file(int file_index, const char* data, int size);
int sys_read_file(int file_index, char* buffer, int size);
int sys_delete_file(int file_index);

#endif // FS_SYSCALLS_H
