# SPDX-License-Identifier: GPL-2.0-only

.global software_isr_wrapper

software_isr_wrapper:
    pushal
    cld              # C code following the sysV ABI requires DF to be clear on function entry
    call software_interrupt_handler
    movb $0x20, %al
    outb %al, $0x20
    popal
    iret
