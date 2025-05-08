# SPDX-License-Identifier: GPL-2.0-only

.global init_paging
.global enable_paging

.section .data
    .align 0x1000

page_directory:
    .fill 1024, 4, 0              # Page Directory: 1024 entries (4KB)

page_tables:
    .fill 1024 * 3, 4, 0          # Only enough for CODE, DATA, RO_DATA (3 page tables)

.section .text

# -------------------------------
# init_paging:
# - Sets up paging for kernel sections
# -------------------------------
init_paging:
    lea page_directory, %ebx           # %ebx = page_directory
    lea page_tables, %edi              # %edi = start of all page tables

    # Map CODE section
    movl $0x00100000, %eax             # Start of CODE section
    movl $0x0, %ecx                    # Page Directory Index (CODE)
    call map_section

    # Map DATA section
    movl $0x06000000, %eax             # Start of DATA section
    movl $1, %ecx                      # Page Directory Index (DATA)
    call map_section

    # Map RO_DATA section
    movl $0x0C000000, %eax             # Start of RO_DATA section
    movl $2, %ecx                      # Page Directory Index (RO_DATA)
    call map_section

    ret

# -------------------------------
# map_section:
# - Maps a 64MB section at %eax
# - Page Directory Index in %ecx
# -------------------------------
map_section:
    # Set page_directory[ecx] = &page_tables[current] | 0x3
    movl %edi, %edx                    # %edx = base addr of current page table
    orl $0x3, %edx                     # mark as present + writable
    movl %edx, (%ebx, %ecx, 4)         # store in page_directory[ecx]

    # Fill page table for the section
    pushl %ecx                         # save outer loop counter
    movl $1024, %ecx                   # 64 MB = 1024 * 4 KB
fill_page_table_loop:
    movl %eax, %edx                    # Physical address to map
    orl $0x3, %edx                     # present + writable
    movl %edx, (%edi)                  # write PTE
    addl $4, %edi                      # next PTE
    addl $0x1000, %eax                 # next physical page (4KB)
    loop fill_page_table_loop

    popl %ecx                          # restore outer loop counter
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
