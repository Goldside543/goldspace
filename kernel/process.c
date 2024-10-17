// SPDX-License-Identifier: GPL-2.0-only
/*
 * kernel/process.c
 *
 * Process handling.
 *
 * Copyright (C) 2024 Goldside543
 *
 */

#include <stdint.h>
#include <stddef.h>
#include "../mm/memory.h"
#include "process.h"

static uint32_t next_pid = 1;  // Static counter for PID generation

pcb_t *current_process = NULL;
pcb_t *process_queue = NULL;

void context_switch(pcb_t *next_process) {
    // Save the current process's state
    asm volatile (
        "mov %%esp, %0\n"
        "mov %%ebp, %1\n"
        : "=m" (current_process->esp), "=m" (current_process->ebp)
    );

    // Load the next process's state
    asm volatile (
        "mov %0, %%esp\n"
        "mov %1, %%ebp\n"
        :
        : "m" (next_process->esp), "m" (next_process->ebp)
    );

    // Jump to the next process's instruction pointer
    asm volatile (
        "jmp *%0\n"
        :
        : "m" (next_process->eip)
    );
}

void schedule() {
    if (process_queue == NULL) {
        return; // No processes to schedule
    }

    // Move to the next process in the queue
    current_process = current_process->next;

    // Perform context switch to the next process
    context_switch(current_process);
}

int generate_pid() {
    return next_pid++;
}

uint32_t* setup_page_directory() {
    // Define PAGE_DIRECTORY_SIZE according to your needs
    #define PAGE_DIRECTORY_SIZE 4096 // Example size

    // Allocate and initialize a page directory
    uint32_t *page_directory = (uint32_t *)kmalloc(PAGE_DIRECTORY_SIZE);
    if (page_directory == NULL) {
        return NULL; // Allocation failed
    }
    // Initialize page directory here
    return page_directory;
}

uint32_t* setup_stack() {
    // Define STACK_SIZE according to your needs
    #define STACK_SIZE 8192 // Example size

    // Allocate memory for the stack
    uint32_t *stack = (uint32_t *)kmalloc(STACK_SIZE);
    if (stack == NULL) {
        return NULL; // Allocation failed
    }
    // Set up the stack pointer
    return stack + STACK_SIZE / sizeof(uint32_t); // Return the top of the stack
}

pcb_t* create_process(void (*entry_point)()) {
    pcb_t *new_pcb = (pcb_t*)kmalloc(sizeof(pcb_t));
    if (new_pcb == NULL) {
        return NULL; // Allocation failed
    }

    new_pcb->pid = generate_pid();
    new_pcb->page_directory = setup_page_directory();
    new_pcb->stack = setup_stack();
    new_pcb->state = PROCESS_RUNNING;
    new_pcb->eip = (uint32_t)entry_point;
    new_pcb->next = NULL;

    // Add to the process queue
    if (process_queue == NULL) {
        process_queue = new_pcb;
        new_pcb->next = new_pcb; // Circular queue for round-robin
    } else {
        pcb_t *temp = process_queue;
        while (temp->next != process_queue) {
            temp = temp->next;
        }
        temp->next = new_pcb;
        new_pcb->next = process_queue;
    }

    return new_pcb;
}

void terminate_process(pcb_t *pcb) {
    if (process_queue == NULL || pcb == NULL) {
        return;
    }

    // Find the PCB in the queue
    pcb_t *current = process_queue;
    pcb_t *prev = NULL;
    do {
        if (current == pcb) {
            if (prev == NULL) { // First PCB in the queue
                // Find the last PCB to maintain circularity
                pcb_t *last = process_queue;
                while (last->next != process_queue) {
                    last = last->next;
                }
                if (current->next == current) {
                    process_queue = NULL;
                } else {
                    process_queue = current->next;
                    last->next = process_queue;
                }
            } else {
                prev->next = current->next;
            }
            kfree(current->page_directory);
            kfree(current->stack);
            kfree(current);
            break;
        }
        prev = current;
        current = current->next;
    } while (current != process_queue);
}

void initialize_process_system() {
    process_queue = NULL; // Initialize the process queue to be empty
    current_process = NULL; // No current process initially
}

void sys_yield() {
    schedule(); // Call the scheduler to switch to the next process
}
