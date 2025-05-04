/* SPDX-License-Identifier: GPL-2.0-only */

ENTRY(kernel_main)

/* Physical load addresses for bootloader */
PHYS_BASE = 0x00100000;

/* Virtual addresses the kernel will use */
VIRT_BASE = 0xC0000000;

MEMORY {
    MULTIBOOT : ORIGIN = 0x00000000, LENGTH = 4K
    CODE      : ORIGIN = VIRT_BASE,     LENGTH = 64M
    DATA      : ORIGIN = VIRT_BASE + 0x01000000, LENGTH = 64M
    RO_DATA   : ORIGIN = VIRT_BASE + 0x02000000, LENGTH = 64M
}

SECTIONS {
    . = VIRT_BASE;

    /* Multiboot header — must be in the first 8KB of the file at physical address */
    .multiboot : AT(PHYS_BASE - 0x1000) ALIGN(4) {
        LONG(0x1BADB002)
        LONG(0x00000000)
        LONG(-(0x1BADB002 + 0x00000000))
    } > MULTIBOOT

    /* Code section */
    .text : AT(ADDR(.text) - VIRT_BASE + PHYS_BASE) ALIGN(16) {
        *(.text)
        *(.text.*)
        *(.rodata)
        *(.rodata.*)
    } > CODE

    /* Data section */
    .data : AT(ADDR(.data) - VIRT_BASE + PHYS_BASE) ALIGN(16) {
        *(.data)
        *(.data.*)
    } > DATA

    /* BSS section */
    .bss : AT(ADDR(.bss) - VIRT_BASE + PHYS_BASE) ALIGN(16) {
        *(.bss)
        *(.bss.*)
        *(COMMON)
    } > DATA

    /* Exception handling stuff */
    .eh_frame : AT(ADDR(.eh_frame) - VIRT_BASE + PHYS_BASE) {
        KEEP(*(.eh_frame))
    } > RO_DATA
}
