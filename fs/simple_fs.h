#ifndef SIMPLE_FS_H
#define SIMPLE_FS_H

#define MAX_FILES 10
#define MAX_FILE_NAME 20
#define BLOCK_SIZE 512
#define NUM_BLOCKS 100

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
