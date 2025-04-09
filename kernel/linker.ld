/* SPDX-License-Identifier: GPL-2.0-only */

ENTRY(kernel_main)

SECTIONS {
    . = 0x00100000;  /* Set kernel base address */

    /* Multiboot header */
    .multiboot : ALIGN(4) {
        LONG(0x1BADB002)
        LONG(0x00000000)  /* Flags (set to 0) */
        LONG(-(0x1BADB002 + 0x00000000))  /* Checksum (negative sum of header fields) */
    }

    /* Code section (.text) */
    .text : ALIGN(16) {
        *(.text)         /* Kernel .text section */
        *(.text.*)       /* Rust code may use additional .text.* sections */
        *(.rodata)       /* Read-only data, often used by Rust */
        *(.rodata.*)     /* Additional read-only data sections from Rust */
    } > CODE

    /* Data section (.data) */
    .data : ALIGN(16) {
        *(.data)         /* Kernel .data section */
        *(.data.*)       /* Rust-generated data sections */
    } > DATA

    /* BSS section (.bss) */
    .bss : ALIGN(16) {
        *(.bss)          /* Kernel .bss section */
        *(.bss.*)        /* Uninitialized data from Rust */
        *(COMMON)
    } > DATA

    /* Exception handling frames (used by Rust, if applicable) */
    .eh_frame : {
        KEEP(*(.eh_frame))  /* Ensure exception handling frames are kept, if present */
    } > RO_DATA

    /* Explicit permissions */
    /* Code section is executable, not writable */
    CODE : { *(.text) *(.text.*) *(.rodata) *(.rodata.*) }
        > CODE
        SECTIONS {
            .text : { *(.text) } > CODE
        }
        /* Make it executable but not writable */
        EXECUTABLE

    /* Data section is writable, not executable */
    DATA : { *(.data) *(.data.*) *(.bss) *(.bss.*) }
        > DATA
        /* Make it writable but not executable */
        RW
}
