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
#include "../mm/memory.h"
#include "../kernel/string.h"

FileSystem fs;

int disk_write(int block_index, const char* data, int size) {
    // Call ata_pio_write and pass along the returned error code
    int write_result = ata_pio_write(block_index * BLOCK_SIZE, data, size);
    return write_result; // 0 for success, or error code from ata_pio_write
}

int disk_read(int block_index, char* buffer, int size) {
    // Call ata_pio_read and pass along the returned error code
    int read_result = ata_pio_read(block_index * BLOCK_SIZE, buffer, size);
    return read_result; // 0 for success, or error code from ata_pio_read
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
    // Validate filename length
    int name_len = 0;
    while (name[name_len] != '\0') name_len++;
    if (name_len >= MAX_FILE_NAME) return -2; // Name is too long

    // Check for duplicate file names to prevent overwrites
    for (int i = 0; i < MAX_FILES; i++) {
        if (my_strcmp(fs.files[i].name, name) == 0) return -3; // File already exists
    }

    // Find a free slot in the file table
    int file_index = -1;
    for (int i = 0; i < MAX_FILES; i++) {
        if (fs.files[i].name[0] == '\0') { // Empty slot found
            file_index = i;
            break;
        }
    }
    if (file_index == -1) return -1; // No space in file table

    // Allocate a block for the new file
    int block_index = -1;
    for (int i = 0; i < NUM_BLOCKS; i++) {
        if (fs.free_blocks[i]) { // Block is free if marked 1
            block_index = i;
            fs.free_blocks[i] = 0; // Mark block as used
            break;
        }
    }
    if (block_index == -1) return -4; // No free blocks available

    // Initialize file metadata
    my_strncpy(fs.files[file_index].name, name, MAX_FILE_NAME - 1); // Safely copy name
    fs.files[file_index].name[MAX_FILE_NAME - 1] = '\0'; // Ensure null-termination
    fs.files[file_index].size = 0; // New file starts empty
    fs.files[file_index].start_block = block_index;

    // Persist the updated file table to disk
    if (disk_write(FILE_TABLE_BLOCK, (void*)&fs, sizeof(fs)) != 0) {
        fs.free_blocks[block_index] = 1; // Roll back block allocation
        fs.files[file_index].name[0] = '\0'; // Roll back file table entry
        return -5; // Failed to write file table to disk
    }

    return file_index; // Return index of new file
}

// Write data to a file
int write_file(int file_index, const char* data, size_t size) {
    // Validate file index to ensure it's within bounds
    if (file_index < 0 || file_index >= MAX_FILES) return -1;

    // Check if file exists by looking at the file's name
    if (fs.files[file_index].name[0] == '\0') return -2;

    // Ensure the data size does not exceed the block size
    if (size > BLOCK_SIZE) return -3; // No need to allocate block if size is too large

    // Locate a free block to store data
    int block_index = -1;
    for (int i = 0; i < NUM_BLOCKS; i++) {
        if (fs.free_blocks[i]) { // Block is free if marked 1
            block_index = i;
            fs.free_blocks[i] = 0; // Mark block as used
            break;
        }
    }

    // Handle case where no free blocks are available
    if (block_index == -1) return -4;

    // Perform the disk write operation with proper error handling
    if (disk_write(block_index, data, size) != 0) {
        fs.free_blocks[block_index] = 1; // Roll back block allocation
        return -5; // Disk write failed
    }

    // Update file metadata only after successful write
    fs.files[file_index].size = size;
    fs.files[file_index].start_block = block_index;

    return 0; // Indicate success
}

int read_file(int file_index, char* buffer, size_t size) {
    // Validate file index range
    if (file_index < 0 || file_index >= MAX_FILES) return -1; // Invalid file index
    
    // Check if file exists
    if (fs.files[file_index].name[0] == '\0') return -2; // File does not exist

    // Verify that the file has data
    int block_index = fs.files[file_index].start_block;
    if (block_index == -1) return -3; // No data written to file

    // Limit read size to the file's actual size
    size_t file_size = fs.files[file_index].size;
    size = (size > file_size) ? file_size : size;

    // Variables for reading data
    size_t bytes_read = 0;
    char temp_buffer[BLOCK_SIZE];

    // Read the data block-by-block
    while (size > 0) {
        // Determine how much of the block to read
        size_t bytes_to_read = (size > BLOCK_SIZE) ? BLOCK_SIZE : size;

        // Read from disk into temp buffer
        if (disk_read(block_index, temp_buffer, BLOCK_SIZE) != 0) return -4; // Disk read error

        // Copy only the relevant part of temp_buffer to the output buffer
        kmemcpy(buffer + bytes_read, temp_buffer, bytes_to_read);

        // Update counters
        bytes_read += bytes_to_read;
        size -= bytes_to_read;
        block_index++; // Move to the next block

        // Stop if we run out of allocated blocks
        if (block_index >= NUM_BLOCKS || fs.free_blocks[block_index] == 1) break;
    }

    return 0; 
}


// Delete a file
int delete_file(int file_index) {
    // Validate file index range
    if (file_index < 0 || file_index >= MAX_FILES) return -1; // Invalid file index

    // Check if file exists
    if (fs.files[file_index].name[0] == '\0') return -2; // File does not exist

    // Free all blocks allocated to this file
    int block_index = fs.files[file_index].start_block;
    while (block_index != -1 && block_index < NUM_BLOCKS) {
        // Optionally, clear the data in each block for security
        char empty_block[BLOCK_SIZE] = {0};
        if (disk_write(block_index, empty_block, BLOCK_SIZE) != 0) return -3; // Disk write error

        // Mark the block as free
        fs.free_blocks[block_index] = 1;

        // Move to the next block (if file spans multiple blocks)
        block_index++; 
        if (fs.free_blocks[block_index] == 1) break; // Stop if unallocated
    }

    // Clear file metadata
    kmemset(fs.files[file_index].name, '\0', MAX_FILE_NAME); // Clear name
    fs.files[file_index].size = 0;
    fs.files[file_index].start_block = -1;

    // Update file system table on disk
    disk_write(FILE_TABLE_BLOCK, (void*)&fs, sizeof(fs));

    return 0; // Success
}
