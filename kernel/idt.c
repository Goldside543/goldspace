// SPDX-License-Identifier: GPL-2.0-only
/*
 * kernel/idt.c
 *
 * Interrupt Descriptor Table.
 *
 * Copyright (C) 2024-2025 Goldside543
 *
 */

#include <stdint.h>
#include "interrupt.h"
#include "print.h"
#include "io.h"
#include "process.h"
#include "panic.h"
#include "time.h"

#define IDT_ENTRIES 256

extern void software_isr_wrapper(void);
extern void keyboard_isr_wrapper(void);
extern void pit_isr_wrapper(void);
extern void gpf_isr_wrapper(void);
extern long saved_cpl;

void gpf_handler() {
    if (saved_cpl == 3) {
        terminate_process(current_process);
        return;
    }
        
    panic("General Protection Fault in the kernel!");
}

void df_handler() {
    panic("Double Fault!");
}

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
    asm volatile("pushal");
    outb(0x20, 0x20);
    asm volatile("popal");
    asm volatile("iret");
}

// Function to set an IDT entry
void set_idt_entry(int interrupt_number, void (*handler)()) {
    idt[interrupt_number].offset_low = (uintptr_t)handler & 0xFFFF;
    idt[interrupt_number].selector = 0x08; // Kernel code segment selector
    idt[interrupt_number].zero = 0;
    idt[interrupt_number].type_attr = 0x8E; // Present, DPL=0, 32-bit interrupt gate
    idt[interrupt_number].offset_high = ((uintptr_t)handler >> 16) & 0xFFFF;
}

void set_idt_entry_exception(int exception_number, void (*handler)()) {
    idt[exception_number].offset_low = (uintptr_t)handler & 0xFFFF;
    idt[exception_number].selector = 0x08; // Kernel code segment selector
    idt[exception_number].zero = 0;
    idt[exception_number].type_attr = 0x8F; // Present, DPL=0, 32-bit trap gate
    idt[exception_number].offset_high = ((uintptr_t)handler >> 16) & 0xFFFF;
}

void irq_clear_mask(uint8_t IRQline) {
    uint16_t port;
    uint8_t value;

    if(IRQline < 8) {
        port = 0x21;
    } else {
        port = 0xA1;
        IRQline -= 8;
    }
    value = inb(port) & ~(1 << IRQline);
    outb(port, value);        
}

int kunk = 0;

void pit_isr() {
    static uint32_t ticks = 0;
    ticks++;

    // PIT is set to fire 250 times per second
    if (ticks >= 250) {
        unix_time++;
        ticks = 0;
    }

    kunk ^= 1;

    schedule();
    outb(0x20, 0x20);
}

void set_idt_entry_syscall(int interrupt_number, void (*handler)()) {
    idt[interrupt_number].offset_low = (uintptr_t)handler & 0xFFFF;
    idt[interrupt_number].selector = 0x08; // Kernel code segment selector
    idt[interrupt_number].zero = 0;
    idt[interrupt_number].type_attr = 0xEE; // Present, DPL=3 (user mode), 32-bit interrupt gate
    idt[interrupt_number].offset_high = ((uintptr_t)handler >> 16) & 0xFFFF;
}


// Function to initialize the IDT
void init_idt() {
    print("Preparing IDT...\n");

    // Set specific IDT entries (e.g., software interrupt)
    set_idt_entry_syscall(0x80, software_isr_wrapper); // Software interrupt for syscalls

    print("Set system call handler.\n");

    set_idt_entry(0x21, keyboard_isr_wrapper); // Hardware interrupt for keyboards

    print("Set keyboard handler.\n");

    set_idt_entry(0x20, pit_isr_wrapper); // Hardware interrupt for PIT

    print("Set PIT handler.\n");

    set_idt_entry_exception(0x0D, gpf_handler); // Fault handler for GPF

    print("Set GPF handler.\n");

    set_idt_entry_exception(0x08, df_handler); // Fault handler for DF

    print("Set DF handler.\n");

    // Prepare the IDT pointer
    struct idt_pointer idtp;
    idtp.limit = (sizeof(struct idt_entry) * IDT_ENTRIES) - 1; // Size of IDT - 1
    idtp.base = (uint32_t)&idt; // Base address of IDT

    print("Loading IDT...\n");
    
    // Load the IDT using the lidt instruction
    asm volatile("lidt %0" : : "m"(idtp));

    outb(0x20, 0x11);  // ICW1 for master PIC: begin initialization, cascade mode
    outb(0xA0, 0x11);  // ICW1 for slave PIC: same for slave PIC

    print("ICW1 set...\n");

    outb(0x21, 0x20);  // ICW2 for master PIC: vector offset 0x20
    outb(0xA1, 0x28);  // ICW2 for slave PIC: vector offset 0x28

    print("ICW2 set...\n");

    outb(0x21, 0x04);  // ICW3 for master PIC: tell it the slave is on IRQ2
    outb(0xA1, 0x02);  // ICW3 for slave PIC: tell it the master is on IRQ2

    print("ICW3 set...\n");
 
    outb(0x21, 0x01);  // ICW4 for master PIC: 8086 mode, no special features
    outb(0xA1, 0x01);  // ICW4 for slave PIC: 8086 mode, no special features

    print("ICW4 set...\n");

    outb(0x21, 0xFF);  // Mask all IRQs on master PIC
    outb(0xA1, 0xFF);  // Mask all IRQs on slave PIC
    irq_clear_mask(1); // Unmask IRQ1 (keyboard)
    irq_clear_mask(0); // Unmask IRQ0 (PIT)

    print("OCW1 set...\n");

}
