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
#define NUM_PAGES (MEMORY_POOL_SIZE / PAGE_SIZE)
#define PAGE_TABLE_SIZE (NUM_PAGES * sizeof(uint32_t))

#define ALIGNMENT 8
#define MAGIC_HEAD 0xDEADBEEF
#define MAGIC_TAIL 0xBAADF00D
#define ALIGN(size) (((size) + (ALIGNMENT - 1)) & ~(ALIGNMENT - 1))

typedef struct {
    uint32_t page_table[NUM_PAGES];
} page_table_t;

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
    free_list = (block_header*)memory_pool;
    free_list->magic_head = MAGIC_HEAD;
    free_list->size = MEMORY_POOL_SIZE - sizeof(block_header) - sizeof(block_footer);
    free_list->free = 1;
    free_list->next = NULL;
    write_footer(free_list);
}

void* kmalloc(size_t size) {
    size = ALIGN(size);
    block_header* current = free_list;

    while (current) {
        if (current->free && current->size >= size) {
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

        current = current->next;
    }

    return NULL; // Out of memory
}

void kfree(void* ptr) {
    if (!ptr) return;

    block_header* block = (block_header*)((uint8_t*)ptr - sizeof(block_header));

    // Check for corruption
    if (block->magic_head != MAGIC_HEAD || get_footer(block)->magic_tail != MAGIC_TAIL) {
        // Corrupted block
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

void *krealloc(void *ptr, size_t new_size) {
    if (!ptr) {
        // If ptr is NULL, just allocate new memory
        return kmalloc(new_size);
    }
    
    if (new_size == 0) {
        // If new_size is 0, free the memory and return NULL
        kfree(ptr);
        return NULL;
    }

    // Allocate new memory block
    void *new_ptr = kmalloc(new_size);
    if (!new_ptr) {
        return NULL;  // Failed to allocate
    }

    // Copy old data to new block (only up to the new size)
    kmemcpy(new_ptr, ptr, new_size);  

    // Free old block
    kfree(ptr);

    return new_ptr;
}

int kmemcmp(const void* ptr1, const void* ptr2, size_t num) {
    const unsigned char* p1 = (const unsigned char*)ptr1;
    const unsigned char* p2 = (const unsigned char*)ptr2;
    for (size_t i = 0; i < num; ++i) {
        if (p1[i] != p2[i]) {
            return p1[i] - p2[i];
        }
    }
    return 0;
}

static page_table_t* page_table;

void page_table_init() {
    page_table = (page_table_t*)kmalloc(sizeof(page_table_t));
    if (page_table == NULL) {
        print("Memory allocation failed during page table initialization.\n");
        return;
    }

    // Initialize page table entries to zero
    for (int i = 0; i < NUM_PAGES; ++i) {
        page_table->page_table[i] = 0;
    }
}

void map_page(uint32_t virtual_address, uint32_t physical_address) {
    uint32_t page_index = virtual_address / PAGE_SIZE;
    page_table->page_table[page_index] = physical_address | 0x3; // Set present and writable bits
}

void kmempaging(void* virtual_address, size_t size) {
    uint32_t start_page = (uint32_t)virtual_address / PAGE_SIZE;
    uint32_t end_page = ((uint32_t)virtual_address + size - 1) / PAGE_SIZE;

    for (uint32_t page = start_page; page <= end_page; ++page) {
        uint32_t physical_address = (uint32_t)kmalloc(PAGE_SIZE);
        if (physical_address == 0) {
            print("Memory allocation failed during paging.\n");
            return;
        }
        map_page(page * PAGE_SIZE, physical_address);
    }
}
