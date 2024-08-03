#ifndef MEMORY_H
#define MEMORY_H

#include <stddef.h>

// Allocate memory of a given size
void* kmalloc(size_t size);

// Free previously allocated memory
void kfree(void* ptr);

#endif // MEMORY_H
