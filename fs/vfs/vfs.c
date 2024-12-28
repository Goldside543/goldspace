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
#include "../fat32/fat32.h"
#include <stddef.h>

// As the kernel is currently limited to a single disk drive, a simple constant set in fs/fs.h will do.

int vfs_create_file(const char* name) {
    int result; // Declare result variable

#if BFFS == 0
    result = create_file(name);  // Create file using the BFFS system
#elif BFFS == 1
    #if FAT32 == 0
        result = fat32_write_file(name, NULL, 0);  // Use FAT32 to create a file
    #elif FAT32 == 1
        result = -1;  // Return -1 if FAT32 is disabled
    #endif
#endif

    return result;  // Return the result of the operation
}

int vfs_write_file(int file_index, const char* data, size_t size) {
    int result; // Declare result variable

#if BFFS == 0
    result = write_file(file_index, data, size);  // Write file using the BFFS system
#elif BFFS == 1
    #if FAT32 == 0
        result = fat32_write_file(file_index, data, size);  // Use FAT32 to write data to a file
    #elif FAT32 == 1
        result = -1;  // Return -1 if FAT32 is disabled
    #endif
#endif

    return result;  // Return the result of the operation
}

int vfs_read_file(int file_index, char* buffer, size_t size) {
    int result; // Declare result variable

#if BFFS == 0
    result = read_file(file_index, buffer, size);  // Read file using the BFFS system
#elif BFFS == 1
    #if FAT32 == 0
        result = fat32_read_file(file_index, buffer, size);  // Use FAT32 to read data from a file
    #elif FAT32 == 1
        result = -1;  // Return -1 if FAT32 is disabled
    #endif
#endif

    return result;  // Return the result of the operation
}

int vfs_delete_file(int file_index) {
    int result; // Declare result variable

#if BFFS == 0
    result = delete_file(file_index);  // Delete file using the BFFS system
#elif BFFS == 1
    #if FAT32 == 0
        result = fat32_delete_file(file_index);  // Use FAT32 to delete a file
    #elif FAT32 == 1
        result = -1;  // Return -1 if FAT32 is disabled
    #endif
#endif

    return result;  // Return the result of the operation
}
