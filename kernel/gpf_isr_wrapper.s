# SPDX-License-Identifier: GPL-2.0-only

.global gpf_isr_wrapper
.global saved_cs

.section .data
saved_cs: .long 0

.section .text
gpf_isr_wrapper:
  popl %eax
  popl %eax
  movl %eax, saved_cs
  and $0x3, saved_cs
  pushal
  cld              # C code following the sysV ABI requires DF to be clear on function entry
  call gpf_handler
  popal
  iret
