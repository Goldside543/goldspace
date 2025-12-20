/* SPDX-License-Identifier: GPL-2.0-only */

ENTRY(kernel_main)

/* Define memory regions */
MEMORY {
    MULTIBOOT : ORIGIN = 0x00000000, LENGTH = 4K
    REALMODE  : ORIGIN = 0x00080000, LENGTH = 32K
    CODE      : ORIGIN = 0x00100000, LENGTH = 8M
    DATA      : ORIGIN = 0x06000000, LENGTH = 8M
    RO_DATA   : ORIGIN = 0x0C000000, LENGTH = 8M
    USERLAND  : ORIGIN = 0x0C800000, LENGTH = 8M
}

/* Define sections */
SECTIONS {
    . = 0x00000000;  /* Set the start of memory to the beginning */

    /* Multiboot header */
    .multiboot : ALIGN(0x1000) {  /* Align to 4KB pages */
        LONG(0x1BADB002)  /* Magic number */
        LONG(0x00000000)  /* Flags (set to 0) */
        LONG(-(0x1BADB002 + 0x00000000))  /* Checksum (negative sum of header fields) */
    } > MULTIBOOT  /* Place Multiboot header in the MULTIBOOT section */

    .realmode : ALIGN(0x10) { /* Align to 16 bytes */
        *(.realmode)
    } > REALMODE

    /* Code section (.text) */
    .text : ALIGN(0x1000) {  /* Align to 4KB pages */
        *(.text)         /* Kernel .text section */
        *(.text.*)       /* Rust code may use additional .text.* sections */
        *(.rodata)       /* Read-only data, often used by Rust */
        *(.rodata.*)     /* Additional read-only data sections from Rust */
    } > CODE

    /* Data section (.data) */
    .data : ALIGN(0x1000) {  /* Align to 4KB pages */
        *(.data)         /* Kernel .data section */
        *(.data.*)       /* Rust-generated data sections */
    } > DATA

    /* BSS section (.bss) */
    .bss : ALIGN(0x1000) {  /* Align to 4KB pages */
        *(.bss)          /* Kernel .bss section */
        *(.bss.*)        /* Uninitialized data from Rust */
        *(COMMON)
    } > DATA

    .userland : ALIGN(0x1000) {
        *(.userland)
    } > USERLAND

    /* Exception handling frames (used by Rust, if applicable) */
    .eh_frame : ALIGN(0x1000) {  /* Align to 4KB pages */
        KEEP(*(.eh_frame))  /* Ensure exception handling frames are kept, if present */
    } > RO_DATA
}
