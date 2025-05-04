# SPDX-License-Identifier: GPL-2.0-only

.global init_paging
.global enable_paging

.section .data
    .align 0x1000            # Align to 4KB boundary for paging structures

# Page Directory: 1024 entries (4 bytes each)
page_directory:
    .fill 1024, 4, 0        # Fill with zeros initially

# Page Tables: 1024 entries for each page directory entry
page_tables:
    .fill 1024*1024, 4, 0   # 1024 page tables, 1024 entries per table, 4 bytes each

.section .text

# init_paging: Initialize page directory and page tables
init_paging:
    # Clear out the page directory and page tables (just in case)
    lea page_directory, %eax  # Load address of page directory
    movl $1024*4, %ecx        # Loop through 1024 entries (4 bytes each)
    xorl %edx, %edx           # Clear register (set entries to 0)
clear_page_directory:
    movl %edx, (%eax)         # Write 0 to current page directory entry
    addl $4, %eax             # Move to next entry
    loop clear_page_directory

    # Set up first page directory entry (maps to first page table)
    lea page_tables, %eax    # Load address of page tables
    movl %eax, page_directory  # Set first page directory entry to page table base (physical address)
    orl $0x3, page_directory  # Set the present and read-write bits (bit 0 and 1)
    addl $4, page_directory   # Move to next entry

    # Initialize page tables
    lea page_tables, %eax    # Load address of page tables
    movl $1024, %ecx          # Loop for 1024 page tables
initialize_page_tables:
    xorl %edx, %edx           # Clear the page table entry
    movl %edx, (%eax)         # Write 0 to current page table entry
    addl $4, %eax             # Move to next entry
    loop initialize_page_tables

    # Set up first page table (maps first 4MB of virtual memory to physical memory)
    lea page_tables, %eax    # Load address of page tables
    movl %eax, page_tables    # Set base address of page table
    orl $0x3, page_tables     # Set present and read-write bits
    ret

# enable_paging: Enable paging by setting the CR0, CR3, and CR4 registers
enable_paging:
    # Set CR3 to the address of page directory
    lea page_directory, %eax
    movl %eax, %cr3           # Set CR3 to page directory base address

    # Enable paging by setting the PG bit in CR0
    movl %cr0, %eax
    orl $0x80000000, %eax
    movl %eax, %cr0
    ret
