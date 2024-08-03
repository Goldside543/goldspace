#ifndef SIMPLE_FS_H
#define SIMPLE_FS_H

#define DISK_SIZE (NUM_BLOCKS * BLOCK_SIZE)
#define NUM_BLOCKS 100
#define BLOCK_SIZE 4096
#define MAX_FILES 50
#define MAX_FILE_NAME 256
#define DISK_BASE_ADDR 0x10000000 // Base address for the disk (example address)


extern FileSystem fs;

// Function declarations
void fs_init();
int create_file(const char* name);
int write_file(int file_index, const char* data, int size);
int read_file(int file_index, char* buffer, int size);
int delete_file(int file_index);

#endif // SIMPLE_FS_H
