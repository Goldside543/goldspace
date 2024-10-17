/* SPDX-License-Identifier: GPL-2.0-only */
/*
 * kernel/ipc.c
 * 
 * Inter-Process Communication (IPC) implementation.
 *
 * Copyright (C) 2024 Goldside543
 *
 */

#include "ipc.h"
#include "../mm/memory.h"

// Create a new pipe
pipe_t* create_pipe(size_t size) {
    pipe_t *pipe = (pipe_t *)kmalloc(sizeof(pipe_t));
    if (!pipe) {
        return NULL; // Allocation failed
    }

    pipe->buffer = (char *)kmalloc(size);
    if (!pipe->buffer) {
        kfree(pipe); // Free the pipe structure
        return NULL; // Allocation failed
    }

    pipe->size = size;
    pipe->read_pos = 0;
    pipe->write_pos = 0;
    pipe->read_lock = 1; // Initialize read lock (unlocked)
    pipe->write_lock = 1; // Initialize write lock (unlocked)

    return pipe;
}

// Write data to the pipe
int pipe_write(pipe_t *pipe, const char *buf, size_t count) {
    if (!pipe || !buf || count == 0) {
        return -1; // Invalid parameters
    }

    // Wait for write lock (pseudo-code)
    while (pipe->write_lock == 0); // Busy wait (replace with a proper semaphore implementation)
    pipe->write_lock = 0;

    for (size_t i = 0; i < count; ++i) {
        pipe->buffer[pipe->write_pos] = buf[i];
        pipe->write_pos = (pipe->write_pos + 1) % pipe->size; // Circular buffer wrap
    }

    // Signal the read lock (pseudo-code)
    pipe->read_lock = 1;

    // Release write lock
    pipe->write_lock = 1;
    return count; // Return the number of bytes written
}

// Read data from the pipe
int pipe_read(pipe_t *pipe, char *buf, size_t count) {
    if (!pipe || !buf || count == 0) {
        return -1; // Invalid parameters
    }

    // Wait for read lock (pseudo-code)
    while (pipe->read_lock == 0); // Busy wait (replace with a proper semaphore implementation)
    pipe->read_lock = 0;

    for (size_t i = 0; i < count; ++i) {
        buf[i] = pipe->buffer[pipe->read_pos];
        pipe->read_pos = (pipe->read_pos + 1) % pipe->size; // Circular buffer wrap
    }

    // Signal the write lock (pseudo-code)
    pipe->write_lock = 1;

    // Release read lock
    pipe->read_lock = 1;
    return count; // Return the number of bytes read
}

// Free the pipe
void free_pipe(pipe_t *pipe) {
    if (pipe) {
        kfree(pipe->buffer);
        kfree(pipe);
    }
}
