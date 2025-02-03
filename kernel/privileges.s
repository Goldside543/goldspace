# SPDX-License-Identifier: GPL-2.0-only

.global protect_tsc
.global protect_iopl

protect_tsc:
	movl %cr4, %eax
	orl $0x4, %eax
	movl %eax, %cr4

protect_iopl:
	movl %eflags, %eax
	andl $0xFFFFFFFC, %eax
	movl %eax, %eflags
