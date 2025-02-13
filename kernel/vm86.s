# SPDX-License-Identifier: GPL-2.0-only

.global enter_vm86

.section .data
vm86_stack:
    .long 0

.section .text
enter_vm86:
    cli                        # Disable interrupts

    movl $0x10, %eax           # Load a real-mode-compatible data segment
    movw %ax, %ds              # Set data segment
    movw %ax, %es
    movw %ax, %fs
    movw %ax, %gs
    movw %ax, %ss

    movl $vm86_stack, %esp     # Set up a stack

    pushf                      # Push original EFLAGS
    orl $0x20000, (%esp)       # Set VM86 mode bit (bit 17 in EFLAGS)
    popf                       # Restore EFLAGS with VM86 mode enabled

    iret
