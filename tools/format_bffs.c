// SPDX-License-Identifier: GPL-2.0-only
/*
 * format_bffs.c
 *
 * User space utility to format a disk with Baby's First File System.
 *
 * Copyright (C) 2024 Goldside543
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#include "../fs/simple_fs.h"

#define BLOCK_SIZE 512  // Adjust this as per your implementation
#define NUM_BLOCKS 1024 // Adjust this according to your file system design

FileSystem fs;

void disk_write(int fd, int block_index, const char* data, int size) {
    lseek(fd, block_index * BLOCK_SIZE, SEEK_SET);
    write(fd, data, size);
}

void clear_disk(int fd) {
    char empty_block[BLOCK_SIZE] = {0}; // Initialize a block of zeros
    for (int i = 0; i < NUM_BLOCKS; i++) {
        disk_write(fd, i, empty_block, BLOCK_SIZE); // Write empty blocks to the disk
    }
}

int format_disk(const char* disk_path) {
    // Open the disk image or device file
    int fd = open(disk_path, O_RDWR);
    if (fd < 0) {
        perror("Failed to open disk");
        return -1;
    }

    // Step 1: Clear the disk space
    clear_disk(fd);

    // Step 2: Initialize the file system structure
    memset(&fs, 0, sizeof(fs)); // Clear the file system structure
    for (int i = 0; i < NUM_BLOCKS; i++) {
        fs.free_blocks[i] = 1; // 1 indicates that the block is free
    }

    // Initialize the file table
    for (int i = 0; i < MAX_FILES; i++) {
        memset(fs.files[i].name, 0, MAX_FILE_NAME);
        fs.files[i].size = 0;
        fs.files[i].start_block = -1;
    }

    // Step 3: Write the initialized file system metadata to disk
    disk_write(fd, FILE_TABLE_BLOCK, (const char*)&fs, sizeof(fs));

    // Close the file descriptor
    close(fd);
    return 0; // Return success
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <disk_image_or_device>\n", argv[0]);
        return EXIT_FAILURE;
    }

    const char* disk_path = argv[1];

    // Format the disk
    if (format_disk(disk_path) < 0) {
        fprintf(stderr, "Failed to format the disk: %s\n", strerror(errno));
        return EXIT_FAILURE;
    }

    printf("Disk %s formatted successfully with Baby's First File System.\n", disk_path);
    return EXIT_SUCCESS;
}
