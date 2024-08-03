#include <stddef.h>
#define MEMORY_POOL_SIZE 1024

static unsigned char memory_pool[MEMORY_POOL_SIZE];
static unsigned int memory_index = 0;

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
