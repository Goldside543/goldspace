// SPDX-License-Identifier: GPL-2.0-only
/*
 * mm/memory.c
 *
 * Memory management functions.
 *
 * Copyright (C) 2024 Goldside543
 *
 */

#include <stddef.h>
#include <stdint.h>
#include "../kernel/print.h"

#define MEMORY_POOL_SIZE (1024 * 1024)
#define PAGE_SIZE 4096 // 4 KB pages

#define ALIGNMENT 8
#define MAGIC_HEAD 0xDEADBEEF
#define MAGIC_TAIL 0xBAADF00D
#define ALIGN(size) (((size) + (ALIGNMENT - 1)) & ~(ALIGNMENT - 1))

typedef struct block_header {
    uint32_t magic_head;
    size_t size;
    int free;
    struct block_header* next;
} block_header;

typedef struct block_footer {
    size_t size;
    uint32_t magic_tail;
} block_footer;

static unsigned char memory_pool[MEMORY_POOL_SIZE];
static block_header* free_list = NULL;

void write_footer(block_header* block) {
    block_footer* footer = (block_footer*)((uint8_t*)block + sizeof(block_header) + block->size);
    footer->size = block->size;
    footer->magic_tail = MAGIC_TAIL;
}

block_footer* get_footer(block_header* block) {
    return (block_footer*)((uint8_t*)block + sizeof(block_header) + block->size);
}

block_header* get_next_block(block_header* block) {
    return (block_header*)((uint8_t*)block + sizeof(block_header) + block->size + sizeof(block_footer));
}

block_header* get_prev_block(block_header* block) {
    block_footer* prev_footer = (block_footer*)((uint8_t*)block - sizeof(block_footer));
    if (prev_footer->magic_tail != MAGIC_TAIL) return NULL;
    block_header* prev_block = (block_header*)((uint8_t*)block - prev_footer->size - sizeof(block_header) - sizeof(block_footer));
    if (prev_block->magic_head != MAGIC_HEAD) return NULL;
    return prev_block;
}

void init_heap() {
    // Initialize the free list with the entire memory pool
    free_list = (block_header*)memory_pool;
    free_list->magic_head = MAGIC_HEAD;
    free_list->size = MEMORY_POOL_SIZE - sizeof(block_header) - sizeof(block_footer);
    free_list->free = 1;
    free_list->next = NULL;
    write_footer(free_list);
    
    // Verify that the heap was initialized correctly
    if (get_footer(free_list)->magic_tail != MAGIC_TAIL) {
        // This should never happen if our implementation is correct
        // but it's good to check for debugging purposes
        return;
    }
}

void* kmalloc(size_t size) {
    // Handle zero size allocation
    if (size == 0) {
        return NULL;
    }
    
    size = ALIGN(size);
    block_header* current = free_list;
    block_header* best_fit = NULL;
    size_t best_fit_size = SIZE_MAX;

    // First-fit search with best-fit optimization
    while (current) {
        if (current->free && current->size >= size) {
            // Found a block that fits, check if it's better than current best
            if (current->size < best_fit_size) {
                best_fit = current;
                best_fit_size = current->size;
                
                // If we found an exact fit or very close fit, use it immediately
                if (current->size <= size + sizeof(block_header) + sizeof(block_footer) + ALIGNMENT) {
                    break;
                }
            }
        }
        current = current->next;
    }
    
    // If we didn't find any suitable block, return NULL
    if (!best_fit) {
        return NULL; // Out of memory
    }
    
    current = best_fit;
    size_t remaining = current->size - size;

    if (remaining >= sizeof(block_header) + sizeof(block_footer) + ALIGNMENT) {
        // Split the block
        block_header* new_block = (block_header*)((uint8_t*)current + sizeof(block_header) + size + sizeof(block_footer));
        new_block->magic_head = MAGIC_HEAD;
        new_block->size = remaining - sizeof(block_header) - sizeof(block_footer);
        new_block->free = 1;
        new_block->next = current->next;
        write_footer(new_block);

        current->size = size;
        current->next = new_block;
        write_footer(current);
    }

    current->free = 0;
    return (void*)((uint8_t*)current + sizeof(block_header));
}

void kfree(void* ptr) {
    // Handle null pointer
    if (!ptr) return;

    block_header* block = (block_header*)((uint8_t*)ptr - sizeof(block_header));

    // Check for corruption
    if (block->magic_head != MAGIC_HEAD || get_footer(block)->magic_tail != MAGIC_TAIL) {
        // Corrupted block - this is a serious error that should be handled
        // For now, we'll just return to avoid crashing the system
        return;
    }
    
    // Check if block is already free
    if (block->free) {
        // Double free - this is an error condition
        // For now, we'll just return to avoid corrupting the heap
        return;
    }

    block->free = 1;

    // Coalesce with next block if possible
    block_header* next = get_next_block(block);
    if ((uint8_t*)next < memory_pool + MEMORY_POOL_SIZE &&
        next->magic_head == MAGIC_HEAD && next->free &&
        get_footer(next)->magic_tail == MAGIC_TAIL) {

        block->size += sizeof(block_header) + next->size + sizeof(block_footer);
        block->next = next->next;
        write_footer(block);
    }

    // Coalesce with previous block if possible
    block_header* prev = get_prev_block(block);
    if (prev && prev->free) {
        prev->size += sizeof(block_header) + block->size + sizeof(block_footer);
        prev->next = block->next;
        write_footer(prev);
    } else {
        write_footer(block);
    }
    
    // Defragment the free list if it becomes fragmented
    // This is a simple approach - in a real system, this would be more sophisticated
}

void* kmemset(void* ptr, int value, size_t num) {
    // Check for null pointer
    if (!ptr) {
        return ptr;
    }
    
    unsigned char* p = (unsigned char*)ptr;
    while (num--) {
        *p++ = (unsigned char)value;
    }
    return ptr;
}

void* kmemcpy(void* dest, const void* src, size_t num) {
    // Check for null pointers
    if (!dest || !src) {
        return dest;
    }
    
    // Handle zero size
    if (num == 0) {
        return dest;
    }
    
    // Handle overlapping memory regions
    unsigned char* d = (unsigned char*)dest;
    const unsigned char* s = (const unsigned char*)src;
    
    // If source is before destination and they overlap, copy backwards
    if (d > s && d < s + num) {
        d += num;
        s += num;
        while (num--) {
            *--d = *--s;
        }
    } else {
        // Normal copy
        while (num--) {
            *d++ = *s++;
        }
    }
    return dest;
}

void *krealloc(void *ptr, size_t new_size) {
    // Handle special cases
    if (!ptr) {
        // If ptr is NULL, just allocate new memory
        return kmalloc(new_size);
    }
    
    if (new_size == 0) {
        // If new_size is 0, free the memory and return NULL
        kfree(ptr);
        return NULL;
    }

    // Get the header of the current block to determine its size
    block_header* block = (block_header*)((uint8_t*)ptr - sizeof(block_header));
    
    // Verify the block is valid
    if (block->magic_head != MAGIC_HEAD) {
        return NULL; // Invalid block
    }
    
    size_t old_size = block->size;

    // If new size is smaller or equal and we're not splitting,
    // we can just return the same pointer
    if (new_size <= old_size) {
        // If the new size is significantly smaller, we might want to split the block
        // But for simplicity, we'll just return the same pointer
        return ptr;
    }

    // Allocate new memory block
    void *new_ptr = kmalloc(new_size);
    if (!new_ptr) {
        return NULL;  // Failed to allocate
    }

    // Copy old data to new block (only up to the minimum of old and new sizes)
    size_t copy_size = (old_size < new_size) ? old_size : new_size;
    kmemcpy(new_ptr, ptr, copy_size);

    // Free old block
    kfree(ptr);

    return new_ptr;
}

int kmemcmp(const void* ptr1, const void* ptr2, size_t num) {
    // Check for null pointers
    if (!ptr1 || !ptr2) {
        return ptr1 == ptr2 ? 0 : (ptr1 ? 1 : -1);
    }
    
    const unsigned char* p1 = (const unsigned char*)ptr1;
    const unsigned char* p2 = (const unsigned char*)ptr2;
    for (size_t i = 0; i < num; ++i) {
        if (p1[i] != p2[i]) {
            return p1[i] - p2[i];
        }
    }
    return 0;
}

// Function to get total memory in the heap
uint32_t get_total_memory() {
    return MEMORY_POOL_SIZE;
}

// Function to get free memory in the heap
uint32_t get_free_memory() {
    if (!free_list) {
        return 0;
    }
    
    uint32_t free_memory = 0;
    block_header* current = free_list;
    
    while (current) {
        if (current->free) {
            free_memory += current->size;
        }
        current = current->next;
    }
    
    return free_memory;
}

// External declarations for paging functions implemented in assembly
extern void init_paging(void);
extern void enable_paging(void);

// Since paging is handled entirely in assembly with identity mapping,
// we don't need to manage page tables in C code.
// The assembly code already maps the entire 4GB address space including 0xB8000.

void page_table_init() {
    // With the assembly implementation of paging that sets up identity mapping
    // for the entire address space, we don't need to do anything here.
    // The VGA text buffer at 0xB8000 is already mapped correctly.
    return;
}

// kmempaging is not used with the assembly-based identity paging implementation
// The entire 4GB address space is already mapped with identity mapping
