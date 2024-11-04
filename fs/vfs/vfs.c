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
    int result; // Declare result variable

#if BFFS == 0
    result = create_file(name); // Use assignment operator
#elif BFFS == 1
    // Behavior not yet defined
    result = -1; // Optional: Return an error code
#endif

    return result; // Return the result of the operation
}

int vfs_write_file(int file_index, const char* data, size_t size) {
    int result; // Declare result variable

#if BFFS == 0
    result = write_file(file_index, data, size);
#elif BFFS == 1
    // Behavior not yet defined
    result = -1;
#endif

    return result; // Return the result of the operation
}

int vfs_read_file(int file_index, char* buffer, size_t size) {
    int result; // Declare result variable

#if BFFS == 0
    result = read_file(file_index, buffer, size);
#elif BFFS == 1
    // Behavior not yet defined
    result = -10;
#endif

    return result; // Return the result of the operation
}

int vfs_delete_file(int file_index) {
    int result; // Declare result variable

#if BFFS == 0
    result = delete_file(file_index);
#elif BFFS == 1
    // Behavior not yet defined
    result = -10;
#endif

    return result; // Return the result of the operation
}
