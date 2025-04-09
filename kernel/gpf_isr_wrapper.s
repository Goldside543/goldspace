# SPDX-License-Identifier: GPL-2.0-only

.global gpf_isr_wrapper
.global saved_cs

.section .data
saved_cs: .long 0
saved_cpl: .long 0
saved_eip: .long 0

.section .text
gpf_isr_wrapper:
  popl %eax
  movl %eax, saved_eip
  popl %eax
  movl %eax, saved_cs
  movl saved_cs, %eax
  movl %eax, saved_cpl
  andl $0x3, saved_cpl
  pushal
  cld              # C code following the sysV ABI requires DF to be clear on function entry
  call gpf_handler
  popal
  movl saved_cs, %eax
  pushl %eax
  movl saved_eip, %eax
  pushl %eax
  iret
