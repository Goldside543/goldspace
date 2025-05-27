/* SPDX-License-Identifier: GPL-2.0-only */

ENTRY(kernel_main)

/* Define memory regions */
MEMORY {
    MULTIBOOT : ORIGIN = 0x00000000, LENGTH = 4K
    REALMODE  : ORIGIN = 0x00080000, LENGTH = 32K
    CODE_PHYS : ORIGIN = 0x00100000, LENGTH = 8M   /* Physical address for kernel code */
    DATA_PHYS : ORIGIN = 0x06000000, LENGTH = 8M   /* Physical address for kernel data */
    RO_DATA_PHYS : ORIGIN = 0x0C000000, LENGTH = 8M/* Physical address for read-only data */
}

/* Define virtual addresses for higher half magic */
KERNEL_VMA = 0xC0000000;
KERNEL_LMA = 0x00100000;

SECTIONS {
    . = 0x00000000;  /* Start from zero for multiboot, just like before */

    .multiboot : ALIGN(0x1000) {
        LONG(0x1BADB002)
        LONG(0x00000000)
        LONG(-(0x1BADB002 + 0x00000000))
    } > MULTIBOOT

    .realmode : ALIGN(0x10) {
        *(.realmode)
    } > REALMODE

    . = KERNEL_VMA;

    .text : AT(KERNEL_LMA) ALIGN(0x1000) {
        *(.text)
        *(.text.*)
        *(.rodata)
        *(.rodata.*)
    } > CODE_PHYS

    .data : AT(LOADADDR(.text) + SIZEOF(.text)) ALIGN(0x1000) {
        *(.data)
        *(.data.*)
    } > DATA_PHYS

    .bss : AT(LOADADDR(.data) + SIZEOF(.data)) ALIGN(0x1000) {
        *(.bss)
        *(.bss.*)
        *(COMMON)
    } > DATA_PHYS

   .eh_frame : AT(LOADADDR(.bss) + SIZEOF(.bss)) ALIGN(0x1000) {
       KEEP(*(.eh_frame))
    } > RO_DATA_PHYS
