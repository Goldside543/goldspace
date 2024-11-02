// SPDX-License-Identifier: GPL-2.0-only
/*
 * kernel/idt.c
 *
 * Interrupt Descriptor Table.
 *
 * Copyright (C) 2024 Goldside543
 *
 */

#include <stdint.h>
#include "interrupt.h"

#define IDT_ENTRIES 256

extern void software_interrupt_handler();

// Define the structure for an IDT entry
struct idt_entry {
    uint16_t offset_low;   // Lower 16 bits of the handler function's address
    uint16_t selector;     // Kernel segment selector
    uint8_t  zero;         // This must always be zero
    uint8_t  type_attr;    // Type and attributes
    uint16_t offset_high;  // Upper 16 bits of the handler function's address
} __attribute__((packed));

// Define the structure for the IDT pointer
struct idt_pointer {
    uint16_t limit;        // Size of the IDT - 1
    uint32_t base;         // Base address of the IDT
} __attribute__((packed));

// The IDT itself
struct idt_entry idt[IDT_ENTRIES];

void default_handler(void) {
   panic("Unknown interrupt.");
}

// Function to set an IDT entry
void set_idt_entry(int interrupt_number, void (*handler)()) {
    idt[interrupt_number].offset_low = (uintptr_t)handler & 0xFFFF;
    idt[interrupt_number].selector = 0x08; // Kernel code segment selector
    idt[interrupt_number].zero = 0;
    idt[interrupt_number].type_attr = 0x8E; // Present, DPL=0, 32-bit interrupt gate
    idt[interrupt_number].offset_high = ((uintptr_t)handler >> 16) & 0xFFFF;
}

// Function to initialize the IDT
void init_idt() {
    // Initialize all IDT entries to default handler
    for (int i = 0; i < IDT_ENTRIES; i++) {
        set_idt_entry(i, default_handler); // Set all to a default handler initially
    }

    // Set specific IDT entries (e.g., software interrupt)
    set_idt_entry(0x80, software_interrupt_handler); // Software interrupt for syscalls

    // Prepare the IDT pointer
    struct idt_pointer idtp;
    idtp.limit = (sizeof(struct idt_entry) * IDT_ENTRIES) - 1; // Size of IDT - 1
    idtp.base = (uint32_t)&idt; // Base address of IDT

    // Load the IDT using the lidt instruction
    asm volatile("lidt %0" : : "m"(idtp));

    asm volatile("sti"); // Set interrupt flag
}
