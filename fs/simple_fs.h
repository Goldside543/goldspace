#ifndef SIMPLE_FS_H
#define SIMPLE_FS_H

#define DISK_SIZE (NUM_BLOCKS * BLOCK_SIZE)
#define NUM_BLOCKS 100
#define BLOCK_SIZE 4096
#define MAX_FILES 50
#define MAX_FILE_NAME 256
#define DISK_BASE_ADDR 0x10000000 // Base address for the disk (example address)
#define SEEK_SET 0
#define SEEK_CUR 1
#define SEEK_END 2
#define O_RDONLY  00
#define O_WRONLY  01
#define O_RDWR    02
#define O_CREAT   0100

typedef struct {
    char name[MAX_FILE_NAME];
    int size;
    int start_block;
} File;

typedef struct {
    char free_blocks[NUM_BLOCKS]; // Use char array for free blocks
    File files[MAX_FILES];
} FileSystem;

extern FileSystem fs;

// Function declarations
void fs_init();
int create_file(const char* name);
int write_file(int file_index, const char* data, int size);
int read_file(int file_index, char* buffer, int size);
int delete_file(int file_index);

#endif // SIMPLE_FS_H
