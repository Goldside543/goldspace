#include "simple_fs.h"
#define DISK_SIZE (NUM_BLOCKS * BLOCK_SIZE)
#define NUM_BLOCKS 100
#define BLOCK_SIZE 4096
#define MAX_FILES 50
#define MAX_FILE_NAME 256
#define DISK_BASE_ADDR 0x10000000 // Base address for the disk (example address)

FileSystem fs;
volatile char* disk = (volatile char*)DISK_BASE_ADDR;

void disk_write(int block_index, const char* data, int size) {
    volatile char* dst = disk + (block_index * BLOCK_SIZE);
    for (int i = 0; i < size; i++) {
        dst[i] = data[i];
    }
}

void disk_read(int block_index, char* buffer, int size) {
    const volatile char* src = disk + (block_index * BLOCK_SIZE);
    for (int i = 0; i < size; i++) {
        buffer[i] = src[i];
    }
}

typedef struct {
    char name[MAX_FILE_NAME];
    int size;
    int start_block;
} File;

typedef struct {
    char free_blocks[NUM_BLOCKS]; // Use char array for free blocks
    File files[MAX_FILES];
} FileSystem;

FileSystem fs;

void fs_init() {
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
    for (int i = 0; i < MAX_FILES; i++) {
        int j;
        for (j = 0; j < MAX_FILE_NAME && name[j] != '\0'; j++) {
            fs.files[i].name[j] = name[j];
        }
        if (j < MAX_FILE_NAME) {
            fs.files[i].name[j] = '\0';
        }

        if (fs.files[i].name[0] == '\0') { // Find an empty slot in the file table
            fs.files[i].size = 0;
            fs.files[i].start_block = -1;
            return i; // Return the index of the new file
        }
    }
    return -1; // No space left in the file table
}

int write_file(int file_index, const char* data, int size) {
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

    // Write data to the block
    disk_write(block_index, data, size);

    fs.files[file_index].size = size;
    fs.files[file_index].start_block = block_index;

    return 0;
}

int read_file(int file_index, char* buffer, int size) {
    if (file_index < 0 || file_index >= MAX_FILES) return -1;
    if (fs.files[file_index].name[0] == '\0') return -1; // File does not exist
    if (fs.files[file_index].start_block == -1) return -1; // No data written to file

    int block_index = fs.files[file_index].start_block;

    // Read data from the block
    disk_read(block_index, buffer, size);

    return 0;
}

int delete_file(int file_index) {
    if (file_index < 0 || file_index >= MAX_FILES) return -1;
    if (fs.files[file_index].name[0] == '\0') return -1; // File does not exist

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
