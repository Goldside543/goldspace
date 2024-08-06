#include <stddef.h>
#include <stdint.h>

#define MEMORY_POOL_SIZE 1024

static unsigned char memory_pool[MEMORY_POOL_SIZE];
static unsigned int memory_index = 0;

/* Allocate memory from the pool */
void* kmalloc(size_t size) {
    if (memory_index + size > MEMORY_POOL_SIZE) {
        return NULL; // Out of memory
    }
    void* ptr = &memory_pool[memory_index];
    memory_index += size;
    return ptr;
}

void kfree(void* ptr) {
    // Simple implementation: Do nothing, as we are not freeing individual blocks
}

void* memset(void* ptr, int value, size_t num) {
    unsigned char* p = (unsigned char*)ptr;
    while (num--) {
        *p++ = (unsigned char)value;
    }
    return ptr;
}

void* memcpy(void* dest, const void* src, size_t num) {
    unsigned char* d = (unsigned char*)dest;
    const unsigned char* s = (const unsigned char*)src;
    while (num--) {
        *d++ = *s++;
    }
    return dest;
}
