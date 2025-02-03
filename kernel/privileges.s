# SPDX-License-Identifier: GPL-2.0-only

.global protect_tsc

protect_tsc:
	movl %cr4, %eax
	orl $0x4, %eax
	movl %eax, %cr4
