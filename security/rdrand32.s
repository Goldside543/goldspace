# SPDX-License-Identifier: GPL-2.0-only

.global rdrand32

rdrand32:
    pushl %ebx
    pushl %ecx

    movl $1, %eax
    cpuid
    
    btl $30, %ecx

    jnc not_set

    rdrand %eax
    popl %ebx
    popl %ecx
    ret
    
not_set:
    xorl %eax, %eax
    popl %ebx
    popl %ecx
    ret
