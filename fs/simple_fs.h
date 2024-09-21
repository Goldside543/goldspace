/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef SIMPLE_FS_H
#define SIMPLE_FS_H

#include <stddef.h>
#include <stdint.h>

// Constants
#define BLOCK_SIZE 512                // Size for each block
#define NUM_BLOCKS 1024               // Total block number
#define MAX_FILES 100                 // Maximum supportable block number
#define MAX_FILE_NAME 256             // Maximum length for File name
#define FILE_TABLE_BLOCK 2            // File table block number

// File system structure
typedef struct {
    char name[MAX_FILE_NAME];         // File name
    size_t size;                      // File size (as byte)
    int32_t start_block;              // Start block index (meanwhile -1 is empty)
} FileEntry;

typedef struct {
    uint8_t free_blocks[NUM_BLOCKS];  // For each block 1: empty, 0: full
    FileEntry files[MAX_FILES];       // File table
} FileSystem;

extern FileSystem fs;

// File system functions
void fs_init(void);                   // Starts FS
int create_file(const char* name);    // Creates File
int write_file(int file_index, const char* data, size_t size); // Writes to the file
int read_file(int file_index, char* buffer, size_t size);      // Reads from file
int delete_file(int file_index);      // Deletes file

#endif // SIMPLE_FS_H
