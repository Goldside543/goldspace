# SPDX-License-Identifier: GPL-2.0-only

.global user_stack
.global jump_usermode
.extern run_user_space

.section .userland
.balign 16                   # Align the following symbol to a 16-byte boundary
user_stack:
    .space 8192              # Allocate 8192 bytes of uninitialized space

.section .text
jump_usermode:
    cli                          # Clear interrupts

    movw $0x23, %ax              # Load 3 into eax
    movw %ax, %ds                # Move the value into ds
    movw %ax, %es                # Move the value into es
    movw %ax, %fs                # Move the value into fs
    movw %ax, %gs                # Move the value into gs

    # Set up the stack frame iret expects
    movl $user_stack + 8192, %eax  # Move current esp into eax
    pushl $0x23                    # Push the data selector (4*8 | 3 = 0x23)
    pushl %eax                     # Push current esp
    pushfl                         # Push flags
    orl $0x200, (%esp)             # Set IF in the saved flags
    pushl $0x1b                    # Push the code selector (3*8 | 3 = 0x1B)
    pushl $run_user_space          # Push the address of the function to return to
    iret
