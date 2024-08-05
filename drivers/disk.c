#include "disk.h"

static char* disk;

// Initialize the disk
void disk_init() {
    // Allocate memory for the disk
    disk = (char*)kmalloc(DISK_SIZE);
    if (disk == NULL) {
        // Handle memory allocation failure
        return;
    }

    // Initialize the disk with zeroes (optional)
    for (int i = 0; i < DISK_SIZE; i++) {
        disk[i] = 0;
    }
}

// Read data from the disk
void disk_read(int block_index, char* buffer, int size) {
    if (block_index < 0 || block_index * BLOCK_SIZE + size > DISK_SIZE) {
        // Handle out-of-bounds error
        return;
    }
    
    // Calculate the starting address
    char* src = disk + (block_index * BLOCK_SIZE);
    
    // Copy data from disk to buffer
    for (int i = 0; i < size; i++) {
        buffer[i] = src[i];
    }
}

// Write data to the disk
void disk_write(int block_index, const char* data, int size) {
    if (block_index < 0 || block_index * BLOCK_SIZE + size > DISK_SIZE) {
        // Handle out-of-bounds error
        return;
    }
    
    // Calculate the starting address
    char* dst = disk + (block_index * BLOCK_SIZE);
    
    // Copy data from buffer to disk
    for (int i = 0; i < size; i++) {
        dst[i] = data[i];
    }
}
