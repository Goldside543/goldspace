#ifndef MEMORY_H
#define MEMORY_H

#include <stddef.h>

// Allocate memory of a given size
void* kmalloc(size_t size);

// Free previously allocated memory
void kfree(void* ptr);

// Set a block of memory to a specific value
void* kmemset(void* ptr, int value, size_t num);

// Copy a block of memory from source to destination
void* kmemcpy(void* dest, const void* src, size_t num);

// Compare two blocks of memory
int kmemcmp(const void* ptr1, const void* ptr2, size_t num);

#endif // MEMORY_H
