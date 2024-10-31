/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef ELF_H
#define ELF_H

#include <stdint.h>

// ELF Identification Indexes
#define EI_MAG0         0   // ELF Magic Number Byte 0
#define EI_MAG1         1   // ELF Magic Number Byte 1
#define EI_MAG2         2   // ELF Magic Number Byte 2
#define EI_MAG3         3   // ELF Magic Number Byte 3
#define EI_CLASS        4   // File Class
#define EI_DATA         5   // Data Encoding
#define EI_VERSION      6   // ELF Version
#define EI_PAD          7   // Start of Padding Bytes

// ELF Magic Number
#define ELFMAG0         0x7F // Magic number byte 0
#define ELFMAG1         'E'  // Magic number byte 1
#define ELFMAG2         'L'  // Magic number byte 2
#define ELFMAG3         'F'  // Magic number byte 3

// ELF Types
#define ET_NONE         0    // No file type
#define ET_REL          1    // Relocatable file
#define ET_EXEC         2    // Executable file
#define ET_DYN          3    // Shared object file
#define ET_CORE         4    // Core file

// ELF Machine Types
#define EM_386          3    // Intel 80386
#define EM_686          6    // Intel 80686

// Program Header Types
#define PT_NULL         0    // Program header table entry unused
#define PT_LOAD         1    // Loadable program segment
#define PT_DYNAMIC      2    // Dynamic linking information
#define PT_INTERP       3    // Program interpreter
#define PT_NOTE         4    // Auxiliary information
#define PT_SHLIB        5    // Reserved
#define PT_PHDR         6    // Entry for header table itself

// Segment Flags
#define PF_X            0x1  // Execute
#define PF_W            0x2  // Write
#define PF_R            0x4  // Read

// ELF Header
typedef struct {
    unsigned char e_ident[16]; // Magic number and other info
    uint16_t      e_type;      // Object file type
    uint16_t      e_machine;   // Architecture
    uint32_t      e_version;   // Object file version
    uint32_t      e_entry;     // Entry point virtual address
    uint32_t      e_phoff;     // Program header table file offset
    uint32_t      e_shoff;     // Section header table file offset
    uint32_t      e_flags;     // Processor-specific flags
    uint16_t      e_ehsize;    // ELF header size in bytes
    uint16_t      e_phentsize; // Program header table entry size
    uint16_t      e_phnum;     // Program header table entry count
    uint16_t      e_shentsize; // Section header table entry size
    uint16_t      e_shnum;     // Section header table entry count
    uint16_t      e_shstrndx;  // Section header string table index
} Elf32_Ehdr;

// Program Header
typedef struct {
    uint32_t p_type;   // Segment type
    uint32_t p_offset; // Segment file offset
    uint32_t p_vaddr;  // Segment virtual address
    uint32_t p_paddr;  // Segment physical address
    uint32_t p_filesz; // Size of segment in file
    uint32_t p_memsz;  // Size of segment in memory
    uint32_t p_flags;  // Segment flags
    uint32_t p_align;  // Segment alignment
} Elf32_Phdr;

#endif // ELF_H
