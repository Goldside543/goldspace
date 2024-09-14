/* SPDX-License-Identifier: GPL-2.0-only
 * kernel/execute.c
 *
 * System call interface for executing a program.
 *
 * Copyright (C) 2024 Goldside543
 *
 */

#include "../mm/memory.h"
#include "../kernel/print.h"
#include "../syscall_table.h"

// Define a structure for holding program details
typedef struct {
    void *code;  // Pointer to the program code
    size_t size; // Size of the program code
    void (*main_function)(void);  // Entry point of the program
} Program;

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

    // Execute the program's main function
    if (main_function) {
        main_function();
    } else {
        print("\n");
        print("Program main function is null!\n");
    }

    // Clean up after execution
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
