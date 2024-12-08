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

// Function to execute an ELF program
void execute_program(const void *program_code, size_t size) {
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
    }

    // Get entry point from ELF header
    void (*entry_point)(void) = (void (*)(void))(elf_header->e_entry);

    // Create a new process for the ELF program
    pcb_t *new_process = create_process(entry_point);
    if (!new_process) {
        print("Failed to create a new process for execution\n");
        return;
    }

    // Context switch to the new process
    context_switch(new_process); // Switch to the new process's context
}

// System call interface for execute_elf_program
int sys_execute_program(void *program_code, void *size, void *unused1, void *unused2) {
    // Cast parameters to the appropriate types
    const void *code = (const void *)program_code;
    size_t program_size = (size_t)(uintptr_t)size;

    // Call the execute_elf_program function
    execute_program(code, program_size);

    // Return a status code (0 for success)
    return 0;
}
