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

    .text : {
        *(.text)
    }

    .rodata : {
        *(.rodata)
    }

    .data : {
        *(.data)
    }

    .bss : {
        *(.bss)
    }
}
