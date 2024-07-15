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
} FileSystem;

FileSystem fs;

#endif // SIMPLE_FS_H
