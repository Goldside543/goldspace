// SPDX-License-Identifier: GPL-2.0-only
/*
 * fs/fs_syscalls.c
 * 
 * File system related system calls.
 *
 * Copyright (C) 2024 Goldside543
 *
 */

#include "../kernel/syscall_numbers.h"
#include "../kernel/print.h"
#include "simple_fs.h" 

// System call handler for creating a file
int sys_create_file(const char* name) {
    int result = create_file(name);
    if(result<0){
        print("No space left in the file table.\n");
    }
}

// System call handler for writing to a file
int sys_write_file(int file_index, const char* data, int size) {
    int result = write_file(file_index, data, size);
    if(result<0){
        print("No free blocks available, data size is too large or file doesn't exist\n"); // Error codes are planned in the future.
    }
}

// System call handler for reading from a file
int sys_read_file(int file_index, char* buffer, int size) {
    int result = read_file(file_index, buffer, size);
    if(result<0){
        print("File doesn't exist or no data written.\n");
    }
}

// System call handler for deleting a file
int sys_delete_file(int file_index) {
    int result = delete_file(file_index);
    if(result<0){
        print("File doesn't exist.\n");
    }
}
