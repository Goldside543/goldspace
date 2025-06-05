# SPDX-License-Identifier: GPL-2.0-only

.global software_isr_wrapper

software_isr_wrapper:
    pushl %esi
    pushl %edx
    pushl %ecx
    pushl %ebx
    pushl %eax

    cld              # C code following the sysV ABI requires DF to be clear on function entry

    call software_interrupt_handler

    addl $20, %esp

    iret
