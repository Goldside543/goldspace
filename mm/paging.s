# SPDX-License-Identifier: GPL-2.0-only

.global init_paging
.global enable_paging

.section .data
    .align 0x1000

# One Page Directory (4KB) = 1024 entries
page_directory:
    .fill 1024, 4, 0

# One Page Table (4KB) = 1024 entries
page_table_0:
    .fill 1024, 4, 0

.section .text

# -------------------------------
# init_paging:
# - Setup identity-mapped paging
# -------------------------------
init_paging:
    # Set up page directory entry 0 to point to page_table_0
    lea page_table_0, %eax         # %eax = &page_table_0
    movl %eax, %edx                # copy address
    orl $0x3, %edx                 # present + writable
    movl %edx, page_directory      # page_directory[0] = page_table_0 | flags

    # Fill page_table_0 with identity-mapped entries
    movl $0, %edi                  # Physical address counter
    lea page_table_0, %eax
    movl $1024, %ecx               # 1024 entries
fill_page_table:
    movl %edi, %edx                # edx = physical addr
    orl $0x3, %edx                 # present + writable
    movl %edx, (%eax)              # write entry
    addl $4, %eax                  # next PTE
    addl $0x1000, %edi             # next physical page (4KB)
    loop fill_page_table

    ret

# -------------------------------
# enable_paging:
# - Load CR3, set PG bit in CR0
# -------------------------------
enable_paging:
    # Load page directory base into CR3
    lea page_directory, %eax
    movl %eax, %cr3

    # Enable paging (set PG bit in CR0)
    movl %cr0, %eax
    orl $0x80000000, %eax          # set PG bit (bit 31)
    movl %eax, %cr0

    ret
