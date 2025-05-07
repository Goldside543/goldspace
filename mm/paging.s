# SPDX-License-Identifier: GPL-2.0-only

.global init_paging
.global enable_paging

.section .data

.align 0x1000
page_directory:
    .fill 1024, 4, 0              # Page Directory: 1024 entries (4KB)

.align 0x1000
page_tables:
    .fill 1024 * 1024, 4, 0       # 1024 Page Tables: each has 1024 entries

.section .text

# -------------------------------
# init_paging:
# - Sets up 4GB identity paging
# -------------------------------
init_paging:
    lea page_directory, %ebx           # %ebx = page_directory
    lea page_tables, %edi              # %edi = start of all page tables
    movl $0, %eax                      # physical address counter
    movl $1024, %ecx                   # 1024 page tables to make
page_dir_loop:
    # Set page_directory[i] = &page_tables[i * 4KB] | 0x3
    movl %edi, %edx                    # %edx = base addr of current page table
    orl $0x3, %edx                     # mark as present + writable
    movl %edx, (%ebx)                  # store in page_directory[i]

    # Now fill the current page table
    pushl %ecx                         # save outer loop counter
    movl $1024, %ecx
fill_page_table_loop:
    movl %eax, %edx                    # physical address
    orl $0x3, %edx                     # present + writable
    movl %edx, (%edi)                  # write PTE
    addl $4096, %edi                   # next entry in PTE
    addl $0x1000, %eax                 # next physical page (4KB)
    loop fill_page_table_loop

    popl %ecx                          # restore outer loop counter
    addl $4, %ebx                      # next PDE
    loop page_dir_loop

    ret

# -------------------------------
# enable_paging:
# - Loads CR3, sets PG bit in CR0
# -------------------------------
enable_paging:
    lea page_directory, %eax
    movl %eax, %cr3                   # load page directory into CR3

    movl %cr0, %eax
    orl $0x80000000, %eax             # enable PG (bit 31)
    movl %eax, %cr0

    ret
