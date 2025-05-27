# SPDX-License-Identifier: GPL-2.0-only

.global init_paging
.global enable_paging

.section .data
.align 0x1000

page_directory:
    .fill 1024, 4, 0

page_tables:
    .fill 1024 * 8, 4, 0        # 8 page tables (8 * 4KB * 1024 = 32MB)

.section .text

# Helper macro
.macro MAP_REGION vaddr, paddr, size
    pushl %eax
    pushl %ebx
    pushl %ecx
    pushl %edx
    pushl %esi
    pushl %edi

    movl $\vaddr, %esi      # virtual
    movl $\paddr, %edi      # physical
    movl $\size, %ecx       # total size to map

.map_loop\@:
    movl %esi, %eax
    shrl $22, %eax                   # page directory index
    movl %eax, %ebx
    shll $2, %ebx                    # * 4 for byte offset
    lea page_directory, %edx
    addl %ebx, %edx                  # &page_directory[index]

    # If page directory entry not set, assign page table
    movl (%edx), %eax
    testl $1, %eax
    jnz .skip_pd\@

    # Find next free page table slot
    movl %eax, %eax                  # zero %eax
    movl %esi, %ebx
    shrl $22, %ebx
    shll $12, %ebx                   # * 0x1000 (size of table)
    lea page_tables, %eax
    addl %ebx, %eax
    orl $0x3, %eax                   # present + RW
    movl %eax, (%edx)

.skip_pd\@:
    # Set page table entry
    movl %esi, %eax
    shrl $12, %eax                   # page table index
    andl $0x3FF, %eax
    shll $2, %eax                    # offset into table

    movl (%edx), %ebx                # get page table addr
    andl $0xFFFFF000, %ebx
    addl %eax, %ebx

    movl %edi, %eax
    orl $0x3, %eax                   # present + RW
    movl %eax, (%ebx)

    addl $0x1000, %esi
    addl $0x1000, %edi
    subl $0x1000, %ecx
    jg .map_loop\@

    popl %edi
    popl %esi
    popl %edx
    popl %ecx
    popl %ebx
    popl %eax
.endm

# -------------------------------
# init_paging
# -------------------------------
init_paging:
    # Clear page directory and page tables
    lea page_directory, %edi
    movl $0, %eax
    movl $1024, %ecx
.clear_pd:
    stosl
    loop .clear_pd

    lea page_tables, %edi
    movl $0, %eax
    movl $1024 * 8, %ecx
.clear_pts:
    stosl
    loop .clear_pts

    # Map identity regions
    MAP_REGION 0x00000000, 0x00000000, 0x1000
    MAP_REGION 0x00080000, 0x00080000, 0x8000
    MAP_REGION 0x00100000, 0x00100000, 0x800000
    MAP_REGION 0x06000000, 0x06000000, 0x800000

    ret

# -------------------------------
# enable_paging
# -------------------------------
enable_paging:
    lea page_directory, %eax
    movl %eax, %cr3         # load page directory

    movl %cr0, %eax
    orl $0x80000000, %eax   # set PG bit
    movl %eax, %cr0

    ret
