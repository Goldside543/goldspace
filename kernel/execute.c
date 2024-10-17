// SPDX-License-Identifier: GPL-2.0-only
/*
 * kernel/execute.c
 *
 * System call interface for executing a program.
 *
 * Copyright (C) 2024 Goldside543
 *
 */

#include "../mm/memory.h"
#include "print.h"
#include "syscall_table.h"
#include "process.h" // Include process header for create_process

// Function to execute a program
void execute_program(const void *program_code, size_t size, void (*main_function)(void)) {
    // Allocate memory for the program
    void *program_memory = kmalloc(size);
    if (!program_memory) {
        print("\n");
        print("Failed to allocate memory for program code\n");
        return;
    }

    // Copy the program code into the allocated memory
    kmemcpy(program_memory, program_code, size);

    // Set up paging for the allocated memory
    kmempaging(program_memory, size);

    // Create a new process for the program
    pcb_t *new_process = create_process(main_function);
    if (!new_process) {
        print("\n");
        print("Failed to create a new process for execution\n");
        kfree(program_memory);
        return;
    }

    // Set the code memory for the new process
    new_process->code = program_memory;

    // Context switch to the new process
    context_switch(new_process); // Switch to the new process's context

    // Clean up after execution (this won't be reached if context_switch is done correctly)
    kfree(program_memory);
}

// System call interface for execute_program
int sys_execute_program(void* program_code, void* size, void* main_function, void* unused) {
    // Cast parameters to the appropriate types
    const void* code = (const void*)program_code;
    size_t program_size = (size_t)(uintptr_t)size;
    void (*func)(void) = (void(*)(void))main_function;

    // Call the execute_program function
    execute_program(code, program_size, func);

    // Return a status code (0 for success)
    return 0;
}
