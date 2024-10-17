/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef PROCESS_H
#define PROCESS_H

#include <stdint.h>

// Process States
#define PROCESS_RUNNING 0
#define PROCESS_WAITING 1
#define PROCESS_TERMINATED 2

// Process Control Block (PCB)
typedef struct process_control_block {
    int pid;                     // Process ID
    void *code;                  // Pointer to the program code
    uint32_t *page_directory;    // Pointer to the page directory for virtual memory
    uint32_t *stack;             // Pointer to the process's stack
    uint32_t state;              // Process state (running, waiting, terminated)
    uint32_t esp, ebp;           // Stack pointers (saved during context switch)
    uint32_t eip;                // Instruction pointer (next instruction to execute)
    struct process_control_block *next; // Pointer to the next PCB in the scheduler queue
} pcb_t;

// Global variables (to be defined in the process.c file)
extern pcb_t *current_process;    // Pointer to the currently running process
extern pcb_t *process_queue;      // Head of the process queue

// Function Prototypes
pcb_t* create_process(void (*entry_point)());
void terminate_process(pcb_t *pcb);
void schedule();
void context_switch(pcb_t *next_process);
int generate_pid();
uint32_t* setup_page_directory();
uint32_t* setup_stack();
void initialize_process_system();

#endif // PROCESS_H
