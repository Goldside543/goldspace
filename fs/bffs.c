// SPDX-License-Identifier: GPL-2.0-only
/*
 * fs/bffs.c
 *
 * Baby's First File System. A (hopefully) portable file system I
 * made just for Goldspace.
 *
 * Copyright (C) 2024 Goldside543
 *
 */

#include "simple_fs.h"
#include "../drivers/disk.h"

FileSystem fs;

void disk_write(int block_index, const char* data, int size) {
    ata_pio_write(block_index * BLOCK_SIZE, data, size);
}

void disk_read(int block_index, char* buffer, int size) {
    ata_pio_read(block_index * BLOCK_SIZE, buffer, size);
}

// Initialize the file system
void fs_init() {
    ata_pio_init();

    // Initialize the free blocks array
    for (int i = 0; i < NUM_BLOCKS; i++) {
        fs.free_blocks[i] = 1; // 1 indicates that the block is free
    }

    // Initialize the file table
    for (int i = 0; i < MAX_FILES; i++) {
        for (int j = 0; j < MAX_FILE_NAME; j++) {
            fs.files[i].name[j] = '\0';
        }
        fs.files[i].size = 0;
        fs.files[i].start_block = -1;
    }
}

int create_file(const char* name) {
    // Check if there's space in the file table
    for (int i = 0; i < MAX_FILES; i++) {
        if (fs.files[i].name[0] == '\0') { // Find an empty slot in the file table
            // Copy the file name
            int j;
            for (j = 0; j < MAX_FILE_NAME && name[j] != '\0'; j++) {
                fs.files[i].name[j] = name[j];
            }
            if (j < MAX_FILE_NAME) {
                fs.files[i].name[j] = '\0'; // Null-terminate the name
            }

            // Allocate a block for the new file
            int block_index = -1;
            for (int k = 0; k < NUM_BLOCKS; k++) {
                if (fs.free_blocks[k] == 1) { // Find a free block
                    block_index = k;
                    fs.free_blocks[k] = 0;    // Mark the block as used
                    break;
                }
            }

            if (block_index == -1) {
                return -1; // No free blocks available
            }

            // Initialize file metadata
            fs.files[i].size = 0;            // File size is 0 at creation
            fs.files[i].start_block = block_index;

            // Persist the updated file table to disk
            disk_write(FILE_TABLE_BLOCK, (void*)&fs, sizeof(fs));

            return i; // Return the index of the new file
        }
    }

    return -1; // No space left in the file table
}

// Write data to a file
int write_file(int file_index, const char* data, size_t size) {
    if (file_index < 0 || file_index >= MAX_FILES) return -1;
    if (fs.files[file_index].name[0] == '\0') return -1; // File does not exist

    // Find a free block
    int block_index = -1;
    for (int i = 0; i < NUM_BLOCKS; i++) {
        if (fs.free_blocks[i] == 1) { // Block is free
            block_index = i;
            fs.free_blocks[i] = 0; // Mark block as used
            break;
        }
    }

    if (block_index == -1) return -1; // No free blocks available

    // Ensure the data size does not exceed the block size
    if (size > BLOCK_SIZE) {
        return -1; // Data size is too large
    }

    // Write data to the block using ATA PIO
    disk_write(block_index, data, size);

    fs.files[file_index].size = size;
    fs.files[file_index].start_block = block_index;

    return 0;
}

// Read data from a file
int read_file(int file_index, char* buffer, size_t size) {
    if (file_index < 0 || file_index >= MAX_FILES) return -1;
    if (fs.files[file_index].name[0] == '\0') return -1; // File does not exist
    if (fs.files[file_index].start_block == -1) return -1; // No data written to file

    int block_index = fs.files[file_index].start_block;

    // Ensure the requested size does not exceed the file size
    if (size > fs.files[file_index].size) {
        size = fs.files[file_index].size; // Adjust size to file size
    }

    // Read data from the block using ATA PIO
    disk_read(block_index, buffer, size);

    return 0;
}

// Delete a file
int delete_file(int file_index) {
    if (file_index < 0 || file_index >= MAX_FILES) return -1;
    if (fs.files[file_index].name[0] == '\0'); // File does not exist

    int block_index = fs.files[file_index].start_block;
    if (block_index != -1) {
        fs.free_blocks[block_index] = 1; // Mark block as free
    }

    // Clear the file entry
    for (int i = 0; i < MAX_FILE_NAME; i++) {
        fs.files[file_index].name[i] = '\0';
    }
    fs.files[file_index].size = 0;
    fs.files[file_index].start_block = -1;

    return 0;
}
