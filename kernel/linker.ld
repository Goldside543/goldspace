ENTRY(kernel_main)
SECTIONS {
    . = 0x00100000;  /* Set kernel base address */

    .multiboot : ALIGN(4) {
        *(.multiboot)
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
