/* SPDX-License-Identifier: GPL-2.0-only */

ENTRY(kernel_main)
SECTIONS {
    . = 0x00100000;  /* Set kernel base address */

    .multiboot : ALIGN(4) {
        /* Multiboot header */
        LONG(0x1BADB002)
        LONG(0x00000000)  /* Flags (set to 0) */
        LONG(- (0x1BADB002 + 0x00000000))  /* Checksum (negative sum of header fields) */

        /* Multiboot header ends here */
    }

    .text : ALIGN(16) {
        *(.text)
    }
    
    .rodata : ALIGN(16) {
        *(.rodata)
    }
    
    .data : ALIGN(16) {
        *(.data)
    }
    
    .bss : ALIGN(16) {
        *(.bss)
    }

}
