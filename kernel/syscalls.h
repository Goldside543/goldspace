/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef SYSCALLS_H
#define SYSCALLS_H

#include <stddef.h>

int create_file(const char* name);
int write_file(int file_index, const char* data, int size);
int read_file(int file_index, char* buffer, int size);
int delete_file(int file_index);

#endif // SYSCALLS_H
