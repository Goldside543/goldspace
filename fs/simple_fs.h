#ifndef SIMPLE_FS_H
#define SIMPLE_FS_H

#define DISK_SIZE (NUM_BLOCKS * BLOCK_SIZE)
#define NUM_BLOCKS 100
#define BLOCK_SIZE 4096
#define MAX_FILES 50
#define MAX_FILE_NAME 256
#define DISK_BASE_ADDR 0x10000000 // Base address for the disk (example address)

// Define a structure to represent a file.
typedef struct {
    char name[MAX_FILE_NAME];
    int size;
    int start_block;
} File;

// Define a structure to represent the file system.
typedef struct {
    File files[MAX_FILES];
    int free_blocks[NUM_BLOCKS];
    char storage[NUM_BLOCKS * BLOCK_SIZE]; // Simulated storage
} FileSystem;

extern FileSystem fs;

// Function declarations
void fs_init();
int create_file(const char* name);
int write_file(int file_index, const char* data, int size);
int read_file(int file_index, char* buffer, int size);
int delete_file(int file_index);

#endif // SIMPLE_FS_H
