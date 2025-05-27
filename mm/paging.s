# SPDX-License-Identifier: GPL-2.0-only

.global init_paging
.global enable_paging

.section .data
    .align 0x1000

page_directory:
    .fill 1024, 4, 0

page_tables:
    .fill 1024 * 8, 4, 0        # Let's allocate 8 page tables for our mapped regions (adjust as needed)

.section .text

# Helper macros for mapping
.macro MAP_REGION vaddr, paddr, size
    # vaddr = virtual start, paddr = physical start, size = bytes
    # Will map [vaddr, vaddr+size) to [paddr, paddr+size)
    # Assumes %ebx = page_directory base, %edi = page_tables base

    pushl %eax
    pushl %ecx
    pushl %edx

    movl $\vaddr, %eax      # Current virtual address
    movl $\paddr, %edx      # Current physical address
    movl $\size, %ecx       # Bytes left to map

.map_page_loop\@:
    movl %eax, %esi
    shrl $22, %esi                  # page directory index
    movl %edi, %ebp
    addl $(4096 * 1024), %ebp       # skip over identity-mapped tables
    movl %ebp, %ebp                 # for clarity
    movl %ebx, %ebp                 # directory base

    # Set up page directory entry for this vaddr
    movl %ebx, %ebp
    movl %esi, %ebp
    shll $2, %ebp
    addl %ebx, %ebp
    movl %ebp, %ebp                 # %ebp = &page_directory[pd_index]

    # Set page directory entry if not present yet
    movl (%ebp), %ebx
    testl $1, %ebx
    jnz .pd_entry_set\@
    # Set page table pointer
    movl %edi, (%ebp)
    orl $0x3, (%ebp)
.pd_entry_set\@:

    # Now set page table entry
    movl %eax, %ebx
    shrl $12, %ebx                  # page table index
    andl $0x3FF, %ebx
    movl (%ebp), %ebp               # get page table address
    andl $0xFFFFF000, %ebp
    addl %ebx, %ebx
    shll $2, %ebx
    addl %ebx, %ebp
    movl %edx, (%ebp)
    orl $0x3, (%ebp)

    # Advance
    addl $0x1000, %eax
    addl $0x1000, %edx
    subl $0x1000, %ecx
    jg .map_page_loop\@

    popl %edx
    popl %ecx
    popl %eax
.endm

# -------------------------------
# init_paging:
# - Sets up identity & higher-half kernel paging
# -------------------------------
init_paging:
    lea page_directory, %ebx
    lea page_tables, %edi

    # Clear page directory and tables
    movl $0, %eax
    movl $1024*4, %ecx
    rep stosl
    movl $0, %eax
    movl $1024*8*4, %ecx     # 8 tables * 1024 entries * 4 bytes
    rep stosl

    # Identity map Multiboot (0x00000000 - 0x00000FFF)
    MAP_REGION 0x00000000, 0x00000000, 0x1000

    # Identity map Realmode (0x00080000 - 0x00087FFF)
    MAP_REGION 0x00080000, 0x00080000, 0x8000

    # Identity map Kernel code (.text, .rodata): 0x00100000 - 0x008FFFFF (8MB)
    MAP_REGION 0x00100000, 0x00100000, 0x800000

    # Identity map Kernel data (.data, .bss): 0x06000000 - 0x067FFFFF (8MB)
    MAP_REGION 0x06000000, 0x06000000, 0x800000

    # Identity map Kernel rodata: 0x0C000000 - 0x0C7FFFFF (8MB)
    MAP_REGION 0x0C000000, 0x0C000000, 0x800000

    # Map kernel to higher-half (virtual 0xC0000000 - 0xC07FFFFF -> physical 0x00100000 - 0x008FFFFF)
    MAP_REGION 0xC0000000, 0x00100000, 0x800000

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
