# SPDX-License-Identifier: GPL-2.0-only

.global init_paging
.global enable_paging
.extern stack_addr
.extern idt_addr

.section .data
    .align 0x1000

page_directory:
    .fill 1024, 4, 0              # Page Directory: 1024 entries (4KB)

page_tables:
    .fill 1024 * 7, 4, 0          # Enough for CODE, DATA, RO_DATA, VGA, stack, IDT, and higher half

.section .text

# -------------------------------
# init_paging:
# - Sets up paging for kernel sections, VGA, stack, IDT, and higher half
# -------------------------------
init_paging:
    lea page_directory, %ebx           # %ebx = page_directory
    lea page_tables, %edi              # %edi = start of all page tables

    # Map CODE section (identity map)
    movl $0x00100000, %eax             # Start of CODE section
    movl $0x0, %ecx                    # Page Directory Index (CODE)
    call map_section

    # Map DATA section (identity map)
    movl $0x06000000, %eax             # Start of DATA section
    movl $1, %ecx                      # Page Directory Index (DATA)
    call map_section

    # Map RO_DATA section (identity map)
    movl $0x0C000000, %eax             # Start of RO_DATA section
    movl $2, %ecx                      # Page Directory Index (RO_DATA)
    call map_section

    # Map VGA framebuffer (identity map @ 0xB80000)
    movl $0xB80000, %eax               # Start of VGA framebuffer
    movl $3, %ecx                      # Page Directory Index (VGA)
    call map_single_page

    # Map stack (identity map @ stack_addr)
    movl stack_addr, %eax              # Start of stack
    movl $4, %ecx                      # Page Directory Index (stack)
    call map_single_page

    # Map IDT (identity map @ idt_addr)
    movl idt_addr, %eax                # Start of IDT
    movl $5, %ecx                      # Page Directory Index (IDT)
    call map_single_page

    # Map CODE section (higher half @ 0xC0000000)
    movl $0x00100000, %eax             # Physical address of CODE
    movl $768, %ecx                    # Page Directory Index (0xC0000000 / 4MB = 768)
    call map_section

    # Map DATA section (higher half @ 0xC4000000)
    movl $0x06000000, %eax             # Physical address of DATA
    movl $769, %ecx                    # Page Directory Index (0xC4000000 / 4MB = 769)
    call map_section

    # Map RO_DATA section (higher half @ 0xC8000000)
    movl $0x0C000000, %eax             # Physical address of RO_DATA
    movl $770, %ecx                    # Page Directory Index (0xC8000000 / 4MB = 770)
    call map_section

    ret

# -------------------------------
# map_section:
# - Maps a 64MB section starting at %eax
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
# map_single_page:
# - Maps a single 4KB page starting at %eax
# - Page Directory Index in %ecx
# -------------------------------
map_single_page:
    # Set page_directory[ecx] = &page_tables[current] | 0x3
    movl %edi, %edx                    # %edx = base addr of current page table
    orl $0x3, %edx                     # mark as present + writable
    movl %edx, (%ebx, %ecx, 4)         # store in page_directory[ecx]

    # Map one page (4KB)
    movl %eax, %edx                    # Physical address to map
    orl $0x3, %edx                     # present + writable
    movl %edx, (%edi)                  # write PTE
    addl $4, %edi                      # next PTE
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
