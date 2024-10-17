/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef IPC_H
#define IPC_H

#include <stddef.h>
#include <stdint.h>

// Define the pipe structure
typedef struct {
    char *buffer;          // Circular buffer
    size_t read_pos;      // Position to read from
    size_t write_pos;     // Position to write to
    size_t size;          // Size of the buffer
    uint32_t read_lock;   // Semaphore for reading
    uint32_t write_lock;  // Semaphore for writing
} pipe_t;

// Function declarations
pipe_t* create_pipe(size_t size);
int pipe_write(pipe_t *pipe, const char *buf, size_t count);
int pipe_read(pipe_t *pipe, char *buf, size_t count);
void free_pipe(pipe_t *pipe);

#endif // IPC_H
