// SPDX-License-Identifier: GPL-2.0-only
/*
 * kernel/execute.c
 *
 * Program execution.
 *
 * Copyright (C) 2024 Goldside543
 *
 */

#include "../mm/memory.h"
#include "../kernel/print.h"

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
    if (!kmempaging(program_memory, size)) {
        print("\n");
        print("Failed to set up paging for program code\n");
        kfree(program_memory);
        return;
    }

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
