#ifndef MEMORY_H
#define MEMORY_H

#include <stddef.h>
#include <stdint.h>

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

// Initialize page tables and setup paging
void page_table_init();

// Map a virtual address to a physical address
void map_page(uint32_t virtual_address, uint32_t physical_address);

// Handle memory paging for a given virtual address and size
void kmempaging(void* virtual_address, size_t size);

#endif // MEMORY_H
