/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef GPU_H
#define GPU_H

#include <stddef.h>

// Define GPU registers and memory locations
#define GPU_BASE_ADDR 0xC0000000
#define GPU_COMMAND_REG (GPU_BASE_ADDR + 0x00)
#define GPU_STATUS_REG (GPU_BASE_ADDR + 0x04)
#define GPU_MEMORY_REG (GPU_BASE_ADDR + 0x08)

// Define GPU commands
#define GPU_CMD_INIT 0x01
#define GPU_CMD_RENDER 0x02

// Structure to represent GPU state
typedef struct {
    void *framebuffer;
    size_t framebuffer_size;
} gpu_state_t;

int gpu_init(void);

void gpu_render(void);

void gpu_cleanup(void);

#endif // GPU_H
