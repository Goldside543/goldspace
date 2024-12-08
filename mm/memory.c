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

#define MEMORY_POOL_SIZE 1024
#define PAGE_SIZE 4096 // 4 KB pages
#define NUM_PAGES (MEMORY_POOL_SIZE / PAGE_SIZE)
#define PAGE_TABLE_SIZE (NUM_PAGES * sizeof(uint32_t))

typedef struct {
    uint32_t page_table[NUM_PAGES];
} page_table_t;

static unsigned char memory_pool[MEMORY_POOL_SIZE];
static unsigned int memory_index = 0;
static unsigned char memory_free_map[MEMORY_POOL_SIZE]; // To track free/used memory

void* kmalloc(size_t size) {
    if (memory_index + size > MEMORY_POOL_SIZE) {
        return NULL; // Out of memory
    }
    void* ptr = &memory_pool[memory_index];
    
    // Mark memory as used
    for (size_t i = 0; i < size; i++) {
        memory_free_map[memory_index + i] = 1;
    }

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

    // Initialize the CR3 register to point to the page table
    uint32_t cr3 = (uint32_t)page_table; // Set CR3 to point to the page table
    asm volatile("mov %0, %%cr3" : : "r"(cr3)); // Write new CR3
}

void enable_paging() {
    // Set the PG bit in CR0 to enable paging
    uint32_t cr0;
    asm volatile("mov %%cr0, %0" : "=r"(cr0));
    cr0 |= (1 << 31); // Set bit 31 (PG) to enable paging
    asm volatile("mov %0, %%cr0" : : "r"(cr0));
    
    // Initialize page table
    page_table_init();

    // Enable paging in the CPU by setting the paging flag in CR0
    asm volatile("mov %%cr0, %0" : "=r"(cr0));
    cr0 |= (1 << 31); // Set PG bit
    asm volatile("mov %0, %%cr0" : : "r"(cr0));

    print("Paging enabled.");

    // Flush the TLB by reloading CR3 with the new page table address
    uint32_t cr3;
    asm volatile("mov %%cr3, %0" : "=r"(cr3)); // Read current CR3
    asm volatile("mov %0, %%cr3" : : "r"(cr3)); // Write it back to flush the TLB

    print("TLB flushed.");
    return;
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
