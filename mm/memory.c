#include <stddef.h>
#include <stdint.h>

#define MEMORY_POOL_SIZE 1024

static unsigned char memory_pool[MEMORY_POOL_SIZE];
static unsigned int memory_index = 0;
static unsigned char memory_free_map[MEMORY_POOL_SIZE]; // To track free/used memory

void* kmalloc(size_t size) {
    if (memory_index + size > MEMORY_POOL_SIZE) {
        return NULL; // Out of memory
    }
    void* ptr = &memory_pool[memory_index];
    memory_index += size;
    return ptr;
}

void kfree(void* ptr) {
    if (ptr == NULL || (unsigned char*)ptr < memory_pool || (unsigned char*)ptr >= memory_pool + MEMORY_POOL_SIZE) {
        return; // Pointer out of bounds or NULL
    }
    // Find the start of the allocated block
    unsigned int index = (unsigned char*)ptr - memory_pool;
    if (index >= MEMORY_POOL_SIZE || memory_free_map[index] == 0) {
        return; // Block not allocated or already freed
    }

    // Mark the block as free
    while (index < MEMORY_POOL_SIZE && memory_free_map[index] == 1) {
        memory_free_map[index] = 0;
        ++index;
    }
}

void* kmemset(void* ptr, int value, size_t num) {
    unsigned char* p = (unsigned char*)ptr;
    while (num--) {
        *p++ = (unsigned char)value;
    }
    return ptr;
}

void* kmemcpy(void* dest, const void* src, size_t num) {
    unsigned char* d = (unsigned char*)dest;
    const unsigned char* s = (const unsigned char*)src;
    while (num--) {
        *d++ = *s++;
    }
    return dest;
}
