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
#include "print.h"

// GDT entries
#define GDT_SIZE 6

// TSS (Task State Segment) structure
struct tss_entry {
    uint32_t prev_task_link;
    uint32_t esp0;
    uint32_t ss0;
    uint32_t esp1;
    uint32_t ss1;
    uint32_t esp2;
    uint32_t ss2;
    uint32_t cr3;
    uint32_t eip;
    uint32_t eflags;
    uint32_t eax, ecx, edx, ebx, esp, ebp, esi, edi;
    uint32_t es, cs, ss, ds, fs, gs;
    uint32_t ldt;
    uint16_t trap;
    uint16_t iobase;
} __attribute__((packed));

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

struct tss_entry tss;

void tss_init() {
    // Initialize the TSS with default values (this will be a minimal setup)
    tss.prev_task_link = 0;
    tss.esp0 = 0;  // Stack pointer for the kernel mode (this should point to the kernel stack)
    tss.ss0 = 0x10; // Kernel data segment
    tss.esp1 = 0;
    tss.ss1 = 0;
    tss.esp2 = 0;
    tss.ss2 = 0;
    tss.cr3 = 0;    // Page directory pointer (use virtual address for paging if needed)
    tss.eip = 0;
    tss.eflags = 0x202;  // Interrupt flag set (IF = 1)
    // General-purpose registers (can be set to 0 for now)
    tss.eax = 0;
    tss.ecx = 0;
    tss.edx = 0;
    tss.ebx = 0;
    tss.esp = 0;
    tss.ebp = 0;
    tss.esi = 0;
    tss.edi = 0;
    // Segment selectors for the current task
    tss.es = 0x10;  // Kernel data segment
    tss.cs = 0x08;  // Kernel code segment
    tss.ss = 0x10;  // Kernel data segment
    tss.ds = 0x10;  // Kernel data segment
    tss.fs = 0x10;  // Kernel data segment
    tss.gs = 0x10;  // Kernel data segment
    tss.ldt = 0;    // No Local Descriptor Table
    tss.trap = 0;
    tss.iobase = 0xFFFF;  // I/O base address
}

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
    print("Setting up GDT...\n");

    // Null descriptor (entry 0)
    gdt_set_entry(0, 0, 0, 0, 0);

    print("Set null descriptor.\n");

    // Kernel code segment (entry 1) - 0x08
    gdt_set_entry(1, 0, 0xFFFFFFFF, 0x9A, 0xCF);

    print("Set kernel code segment.\n");

    // Kernel data segment (entry 2) - 0x10
    gdt_set_entry(2, 0, 0xFFFFFFFF, 0x92, 0xCF);

    print("Set kernel data segment.\n");

    // User code segment (entry 3) - 0x18
    gdt_set_entry(3, 0, 0xFFFFFFFF, 0xFA, 0xCF);

    print("Set user code segment.\n");

    // User data segment (entry 4) - 0x20
    gdt_set_entry(4, 0, 0xFFFFFFFF, 0xF2, 0xCF);

    print("Set user data segment.\n");

    // TSS descriptor (entry 5) - 0x28
    init_tss();
    gdt_set_entry(5, (uint32_t)&tss, sizeof(struct tss_entry), 0x89, 0x40);  // Access flags for TSS descriptor

    print("Set TSS descriptor.\n");

    // Set up the GDT pointer
    gdtp.limit = (sizeof(gdt) - 1);
    gdtp.base = (uint32_t)&gdt;

    print("Loading GDT...\n");

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
        "next:\n"
        :
        : "r" (&gdtp)
        : "%ax", "%bx"
    );
}
