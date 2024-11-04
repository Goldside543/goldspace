// SPDX-License-Identifier: GPL-2.0-only
/*
 * fs/vfs/vfs.c
 *
 * Simple virtual file system layer I'm implementing.
 *
 * Copyright (C) 2024 Goldside543
 *
 */

#include "../fs.h"
#include "../bffs/bffs.h"

/* As the kernel is currently limited to a single
   disk drive, a simple constant set in fs/fs.h 
   will do.
*/

int vfs_create_file(const char* name) {
#if BFFS == 0
    create_file(name); // Argument passed to vfs_create_file should be passed to create_file
#if BFFS == 1
    // Behavior not yet defined
#endif
}

int vfs_write_file(int file_index, const char* data, size_t size) {
#if BFFS == 0
    write_file(file_index, data, size);
#if BFFS == 1
    // Behavior not yet defined
#endif
}

int vfs_read_file(int file_index, char* buffer, size_t size) {
#if BFFS == 0
    read_file(file_index, buffer, size);
#if BFFS == 1
    // Behavior not yet defined
#endif
}

int vfs_delete_file(int file_index) {
#if BFFS == 0
    delete_file(file_index);
#if BFFS == 1
    // Behavior not yet defined
#endif
}
