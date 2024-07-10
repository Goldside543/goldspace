#ifndef MULTIBOOT_H
#define MULTIBOOT_H

#include <stdint.h>

typedef struct multiboot_header {
    uint32_t magic;          // Must be 0x1BADB002
    uint32_t flags;          // Requested features
    uint32_t checksum;       // Verification checksum
    uint32_t header_addr;    // Virtual address of header
    uint32_t load_addr;      // Virtual address kernel should be loaded at
    uint32_t load_end_addr;  // Virtual address of end of kernel
    uint32_t bss_end_addr;   // Virtual address of end of the BSS
    uint32_t entry_addr;     // Entry point of the kernel
} multiboot_header_t;

#endif /* MULTIBOOT_H */
