#ifndef SIMPLE_FS_H
#define SIMPLE_FS_H

#include <stddef.h>

// Constants
#define BLOCK_SIZE 512
#define NUM_BLOCKS 1024
#define MAX_FILES 100
#define MAX_FILE_NAME 256

// File system structure
typedef struct {
    char name[MAX_FILE_NAME];
    int size;
    int start_block;
} FileEntry;

typedef struct {
    int free_blocks[NUM_BLOCKS];
    FileEntry files[MAX_FILES];
} FileSystem;

// File system functions
void fs_init(void);
int create_file(const char* name);
int write_file(int file_index, const char* data, int size);
int read_file(int file_index, char* buffer, int size);
int delete_file(int file_index);

#endif // SIMPLE_FS_H
