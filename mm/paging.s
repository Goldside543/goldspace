# SPDX-License-Identifier: GPL-2.0-only

.global init_paging
.global enable_paging

.section .data
    .align 0x1000

page_directory:
    .fill 1024, 4, 0              # Whole page directory

page_tables:
    .fill 1024 * 16, 4, 0         # 16 page tables only (for 64MB)

.section .text

init_paging:
    lea page_directory, %ebx      # %ebx = page_directory
    lea page_tables, %edi         # %edi = page_tables start
    movl $0, %eax                 # %eax = physical address
    movl $16, %ecx                # we only want 16 page tables (64MB)

page_dir_loop:
    movl %edi, %edx               # %edx = current PT addr
    orl $0x3, %edx                # present + writable
    movl %edx, (%ebx)             # set page_directory[i]

    pushl %ecx                    # save outer loop counter
    movl $1024, %ecx              # 1024 PTEs per PT

fill_page_table_loop:
    movl %eax, %edx               # %edx = physical address
    orl $0x3, %edx                # mark PTE present + writable
    movl %edx, (%edi)             # write PTE
    addl $4, %edi                 # next PTE entry
    addl $0x1000, %eax            # next physical page
    loop fill_page_table_loop

    popl %ecx                     # restore outer loop counter
    addl $4, %ebx                 # next PDE
    loop page_dir_loop

    ret

enable_paging:
    lea page_directory, %eax
    movl %eax, %cr3               # load page directory

    movl %cr0, %eax
    orl $0x80000000, %eax         # set PG bit
    movl %eax, %cr0

    ret
