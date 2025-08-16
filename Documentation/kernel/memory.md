# Memory Management in Goldspace

## Overview

The Goldspace kernel implements a custom memory management system with both heap allocation and virtual memory paging.

## Heap Management

The heap allocator uses a first-fit algorithm with explicit free lists:

### Data Structures

- `block_header`: Contains metadata for each memory block
- `block_footer`: Contains size information and magic number for corruption detection
- `free_list`: Linked list of free memory blocks

### Functions

- `kmalloc(size_t size)`: Allocates a block of memory of the specified size
- `kfree(void* ptr)`: Frees a previously allocated block
- `krealloc(void* ptr, size_t new_size)`: Resizes an allocated block
- `init_heap()`: Initializes the heap allocator

### Memory Layout

The heap is located in a statically allocated array of 1MB size.

## Paging System

The kernel implements a two-level paging system with 4KB pages:

### Page Directory and Page Tables

- Page directory with 1024 entries
- Page tables with 1024 entries each
- Identity mapping for the first 4MB of memory

### Functions

- `paging_init()`: Initializes and enables paging
- `map_page()`: Maps a virtual address to a physical address (not currently used)

### Virtual Memory Layout

The kernel maps the first 4MB of physical memory to virtual addresses starting at 0xC0000000.

## Memory Corruption Detection

The allocator uses magic numbers to detect memory corruption:

- `MAGIC_HEAD` (0xDEADBEEF) at the beginning of each block
- `MAGIC_TAIL` (0xBAADF00D) at the end of each block

## Future Improvements

Planned enhancements include:

- Support for larger allocations
- Better memory fragmentation handling
- More sophisticated paging features