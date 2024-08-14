#include "../mm/memory.h"
#include "../kernel/io.h"
#include "../kernel/print.h"
#include "gpu.h"

// Global GPU state
static gpu_state_t gpu_state;

// Write a byte to an I/O port
static void gpu_outb(uint16_t port, uint8_t value) {
    outb(port, value);
}

// Read a byte from an I/O port
static uint8_t gpu_inb(uint16_t port) {
    return inb(port);
}

// Initialize the GPU
int gpu_init() {
    print("Loading GPU driver...\n");
    // Send initialization command to GPU
    gpu_outb(GPU_COMMAND_REG, GPU_CMD_INIT);
    
    // Check for initialization success (simplified)
    if (gpu_inb(GPU_STATUS_REG) != 0) {
        print("GPU initialization failed.\n")
        return -1;  // Initialization failed
    }
    
    // Allocate framebuffer memory
    gpu_state.framebuffer_size = 1024 * 1024;  // 1MB
    gpu_state.framebuffer = kmalloc(gpu_state.framebuffer_size);
    if (!gpu_state.framebuffer) {
        print("Memory allocation to GPU failed.\n");
        return -1;  // Memory allocation failed
    }
    
    // Set framebuffer address in GPU
    gpu_outb(GPU_MEMORY_REG, (uint8_t)((uintptr_t)gpu_state.framebuffer));

    print("GPU driver loaded.\n");
    return 0;  // Initialization succeeded
}

// Render a simple 2D shape (e.g., a rectangle)
void gpu_render() {
    // Define rendering command
    uint8_t render_command = (0x01 << 6) |  // Command type
                             (0x00 << 4) |  // X position
                             (0x00 << 2) |  // Y position
                             (0xFF);        // Color
    
    // Send render command to GPU
    gpu_outb(GPU_COMMAND_REG, render_command);
    
    // Wait for rendering to complete (simplified)
    while (gpu_inb(GPU_STATUS_REG) != 0) {
        // Polling for completion
    }
}

// Cleanup GPU resources
void gpu_cleanup() {
    // Free framebuffer memory
    kfree(gpu_state.framebuffer);
    
    // Send cleanup command to GPU (if necessary)
    gpu_outb(GPU_COMMAND_REG, 0xFF);
}
