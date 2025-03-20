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
#include "elf.h"     // Include ELF parsing structures and definitions
#include "../fs/vfs/vfs.h"

// Function to execute an ELF program
void execute_program(const void *program_code, size_t size, char **argv) {
    Elf32_Ehdr *elf_header = (Elf32_Ehdr *)program_code;
    
    // Verify ELF magic number
    if (elf_header->e_ident[EI_MAG0] != ELFMAG0 || 
        elf_header->e_ident[EI_MAG1] != ELFMAG1 || 
        elf_header->e_ident[EI_MAG2] != ELFMAG2 || 
        elf_header->e_ident[EI_MAG3] != ELFMAG3) {
        print("Invalid ELF file format\n");
        return;
    }

    // Allocate memory for each ELF segment
    for (int i = 0; i < elf_header->e_phnum; i++) {
        Elf32_Phdr *prog_header = (Elf32_Phdr *)((uint8_t *)program_code + elf_header->e_phoff + i * elf_header->e_phentsize);

        if (prog_header->p_type != PT_LOAD) continue;

        // Allocate and map memory for the segment
        void *segment_memory = kmalloc(prog_header->p_memsz);
        if (!segment_memory) {
            print("Failed to allocate memory for ELF segment\n");
            return;
        }

        kmemcpy(segment_memory, (const uint8_t *)program_code + prog_header->p_offset, prog_header->p_filesz);
        kmempaging(segment_memory, prog_header->p_memsz);
    }

    // Get entry point from ELF header
    void (*entry_point)(void) = (void (*)(void))(elf_header->e_entry);

    // Allocate space for arguments (argv) on the new process's stack
    size_t argc = 0;
    while (argv[argc] != NULL) {
        argc++;
    }

    // Create a new process for the ELF program
    pcb_t *new_process = create_process(entry_point);
    if (!new_process) {
        print("Failed to create a new process for execution\n");
        return;
    }

    // Push arguments onto the stack (simplified version)
    // Assume the process's stack is growing downward
    void **stack = kmalloc(sizeof(void *) * (argc + 1));  // +1 for NULL termination
    for (int i = argc - 1; i >= 0; i--) {
        stack[i] = argv[i];  // Copy the pointer to argv[i] into the stack
    }
    stack[argc] = NULL;  // NULL-terminate argv

    // Set the new process's stack pointer to point to the arguments
    new_process->stack = (void *)stack;

    // Context switch to the new process
    context_switch(new_process); // Switch to the new process's context
}

#define O_RDONLY 0 // For now

// System call interface for execute_elf_program
int sys_execv(void *path, void *argv, void *unused1, void *unused2) {
    // Open the file
    int code_fd = vfs_open(path, O_RDONLY, NULL, NULL);
    if (code_fd < 0) {
        return -1; // Failed to open file
    }

    // Get the file size (assuming vfs_stat exists)
    struct stat st;
    if (vfs_stat(path, &st) < 0) {
        vfs_close(code_fd, NULL, NULL, NULL);
        return -1; // Failed to get file size
    }
    size_t program_size = st.st_size;

    // Allocate memory for the program
    void *code = kmalloc(program_size);
    if (!code) {
        vfs_close(code_fd, NULL, NULL, NULL);
        return -1; // Memory allocation failed
    }

    // Read the file into memory
    ssize_t bytes_read = vfs_read(code_fd, code, program_size, NULL);
    vfs_close(code_fd, NULL, NULL, NULL);  // Close file after reading
    if (bytes_read < 0 || (size_t)bytes_read != program_size) {
        kfree(code);
        return -1; // Read failed
    }

    // Execute the program with arguments (argv passed here)
    execute_program(code, program_size, (char **)argv);

    // Free memory after execution (if execute_program doesn't take ownership)
    kfree(code);

    return 0; // Success
}
