#include "../kernel/syscall_numbers.h"
#include "simple_fs.h" 

// System call handler for creating a file
int sys_create_file(const char* name) {
    return create_file(name);
}

// System call handler for writing to a file
int sys_write_file(int file_index, const char* data, int size) {
    return write_file(file_index, data, size);
}

// System call handler for reading from a file
int sys_read_file(int file_index, char* buffer, int size) {
    return read_file(file_index, buffer, size);
}

// System call handler for deleting a file
int sys_delete_file(int file_index) {
    return delete_file(file_index);
}
