# SPDX-License-Identifier: GPL-2.0-only

.global enable_paging

enable_paging:
    lea page_directory, %eax
    movl %eax, %cr3                   # load page directory into CR3

    movl %cr0, %eax
    orl $0x80000000, %eax             # enable PG (bit 31)
    movl %eax, %cr0

    ret
