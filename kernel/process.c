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
#include "../security/aslr.h"

static uint32_t next_pid = 1;  // Static counter for PID generation

pcb_t *current_process = NULL;
pcb_t *process_queue = NULL;

void context_switch(pcb_t *next_process) {
    // Check for null pointer
    if (!next_process) {
        return;
    }
    
    // Only save current process state if there is a current process
    if (current_process != NULL) {
        // Save the current process's state
        asm volatile (
            "mov %%esp, %0\n"
            "mov %%ebp, %1\n"
            : "=m" (current_process->esp), "=m" (current_process->ebp)
        );
    }

    // Update current process pointer
    current_process = next_process;

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
    // Check if process queue is initialized
    if (process_queue == NULL) {
        return; // No processes to schedule
    }
    
    // If no current process, start with the first one in the queue
    if (current_process == NULL) {
        current_process = process_queue;
    } else {
        // Move to the next process in the queue
        current_process = current_process->next;
        
        // If we've reached the end of the queue, wrap around to the beginning
        if (current_process == NULL) {
            current_process = process_queue;
        }
        
        // Skip any terminated processes
        while (current_process->state == PROCESS_TERMINATED) {
            current_process = current_process->next;
            if (current_process == NULL) {
                current_process = process_queue;
            }
            // If we've gone through the entire queue and all processes are terminated,
            // there's nothing to schedule
            if (current_process->state == PROCESS_TERMINATED && current_process->next == process_queue) {
                current_process = NULL;
                return;
            }
        }
    }

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
    
    // Initialize page directory entries to zero
    for (int i = 0; i < PAGE_DIRECTORY_SIZE / sizeof(uint32_t); i++) {
        page_directory[i] = 0;
    }
    
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
    
    // Initialize stack to zero
    for (int i = 0; i < STACK_SIZE / sizeof(uint32_t); i++) {
        stack[i] = 0;
    }
    
    // Set up the stack pointer to point to the top of the stack
    return stack + STACK_SIZE / sizeof(uint32_t); // Return the top of the stack
}

pcb_t* create_process(void (*entry_point)()) {
    // Check for valid entry point
    if (!entry_point) {
        return NULL;
    }
    
    pcb_t *new_pcb = (pcb_t*)kmalloc(sizeof(pcb_t));
    if (new_pcb == NULL) {
        return NULL; // Allocation failed
    }

    new_pcb->pid = generate_pid();

    // Randomize the page directory address for ASLR
    new_pcb->page_directory = (uint32_t *)generate_random_address();
    if (!new_pcb->page_directory) {
        kfree(new_pcb);
        return NULL; // Randomized allocation failed
    }

    // Randomize the stack address for ASLR
    new_pcb->stack = (uint32_t *)generate_random_address();
    if (!new_pcb->stack) {
        kfree(new_pcb->page_directory);
        kfree(new_pcb);
        return NULL; // Randomized stack allocation failed
    }

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
    // Check for valid parameters
    if (!pcb || process_queue == NULL) {
        return;
    }

    // Find the PCB in the queue
    pcb_t *current = process_queue;
    pcb_t *prev = NULL;
    do {
        if (current == pcb) {
            // Found the PCB to terminate
            if (prev == NULL) { // First PCB in the queue
                // Find the last PCB to maintain circularity
                pcb_t *last = process_queue;
                while (last->next != process_queue) {
                    last = last->next;
                }
                if (current->next == current) {
                    // Only one process in the queue
                    process_queue = NULL;
                } else {
                    process_queue = current->next;
                    last->next = process_queue;
                }
            } else {
                prev->next = current->next;
                // If we're removing the last element, update the previous element's next pointer
                if (current->next == process_queue) {
                    prev->next = process_queue;
                }
            }
            
            // Free allocated memory
            if (current->page_directory) {
                kfree(current->page_directory);
            }
            if (current->stack) {
                kfree(current->stack);
            }
            kfree(current);
            
            // If we just terminated the current process, update current_process
            if (current_process == pcb) {
                current_process = process_queue; // Point to the next process or NULL
            }
            
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

// Function to get information about a process
int get_process_info(int pid, pcb_t* info) {
    // Check for valid parameters
    if (!info || process_queue == NULL) {
        return -1; // Invalid parameters
    }
    
    // Search for the process with the given PID
    pcb_t *current = process_queue;
    do {
        if (current->pid == pid) {
            // Found the process, copy its information
            *info = *current;
            return 0; // Success
        }
        current = current->next;
    } while (current != process_queue);
    
    return -1; // Process not found
}

// Function to get the number of active processes
int get_process_count() {
    if (process_queue == NULL) {
        return 0;
    }
    
    int count = 0;
    pcb_t *current = process_queue;
    do {
        if (current->state != PROCESS_TERMINATED) {
            count++;
        }
        current = current->next;
    } while (current != process_queue);
    
    return count;
}

void sys_yield() {
    schedule(); // Call the scheduler to switch to the next process
}

void sys_exit() {
    // Check if current_process is valid
    if (current_process == NULL) {
        return;
    }

    // Terminate the current process
    terminate_process(current_process);

    // Schedule the next process in the queue
    schedule();
}
