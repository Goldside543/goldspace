// SPDX-License-Identifier: GPL-2.0-only
/*
 * kernel/gdt.c
 *
 * My GDT implementation.
 *
 * Copyright (C) 2024 Goldside543
 *
 */

#include <stdint.h>

// GDT entries
#define GDT_SIZE 6

// GDT structure
struct gdt_entry {
    uint16_t limit_low;
    uint16_t base_low;
    uint8_t base_middle;
    uint8_t access;
    uint8_t granularity;
    uint8_t base_high;
} __attribute__((packed));

// GDT pointer structure
struct gdt_ptr {
    uint16_t limit;
    uint32_t base;
} __attribute__((packed));

// Define the GDT
struct gdt_entry gdt[GDT_SIZE];

// Define the GDT pointer (for loading the GDT)
struct gdt_ptr gdtp;

// Function to set up a GDT entry
void gdt_set_entry(int num, uint32_t base, uint32_t limit, uint8_t access, uint8_t granularity) {
    gdt[num].limit_low = (limit & 0xFFFF);
    gdt[num].base_low = (base & 0xFFFF);
    gdt[num].base_middle = (base >> 16) & 0xFF;
    gdt[num].access = access;
    gdt[num].granularity = (granularity & 0xF0) | ((limit >> 16) & 0x0F);
    gdt[num].base_high = (base >> 24) & 0xFF;
}

// Function to set up the GDT and load it into the CPU
void gdt_init() {
    // Null descriptor (entry 0)
    gdt_set_entry(0, 0, 0, 0, 0);

    // Kernel code segment (entry 1) - 0x08
    gdt_set_entry(1, 0, 0xFFFFFFFF, 0x9A, 0xCF);

    // Kernel data segment (entry 2) - 0x10
    gdt_set_entry(2, 0, 0xFFFFFFFF, 0x92, 0xCF);

    // User code segment (entry 3) - 0x18
    gdt_set_entry(3, 0, 0xFFFFFFFF, 0xFA, 0xCF);

    // User data segment (entry 4) - 0x20
    gdt_set_entry(4, 0, 0xFFFFFFFF, 0xF2, 0xCF);

    // TSS descriptor (entry 5) - 0x28
    gdt_set_entry(5, 0, 0x67, 0x89, 0x40);  // Minimal example

    // Set up the GDT pointer
    gdtp.limit = (sizeof(gdt) - 1);
    gdtp.base = (uint32_t)&gdt;

    // Load the GDT using inline assembly
    asm volatile(
        "cli\n"
        "lgdt (%0)\n"     // Load GDT register with address of GDT pointer
        "mov $0x10, %%ax\n"  // Load code segment selector (kernel)
        "mov %%ax, %%ds\n"   // Load data segment (kernel)
        "mov %%ax, %%es\n"   // Load extra segment
        "mov %%ax, %%fs\n"   // Load fs segment
        "mov %%ax, %%gs\n"   // Load gs segment
        "mov $0x08, %%ax\n"  // Load code segment (kernel)
        "mov %%ax, %%ss\n"   // Load stack segment (kernel)
        "jmp $0x08, $next\n" // Jump to flush old code selector
        "sti\n"
        "next:\n"
        :
        : "r" (&gdtp)
        : "%ax", "%bx"
    );
}
