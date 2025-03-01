.global jump_usermode
.extern run_user_space

jump_usermode:
    movl $0x3, %eax               # Load 3 into eax
    orl $0x20, %eax               # Set the lower 2 bits for ring 3 (0x3 = 00000011b, 0x20 = 00100000b)
    movl %eax, %ds                # Move the value into ds
    movl %eax, %es                # Move the value into es
    movl %eax, %fs                # Move the value into fs
    movl %eax, %gs                # Move the value into gs

    # Set up the stack frame iret expects
    movl %esp, %eax                # Move current esp into eax
    pushl $0x23                    # Push the data selector (4*8 | 3 = 0x23)
    pushl %eax                     # Push current esp
    pushfl                         # Push flags
    cld                            # C code following the sysV ABI requires DF to be clear on function entry
    pushl $0x1b                    # Push the code selector (3*8 | 3 = 0x1B)
    pushl $run_user_space          # Push the address of the function to return to
    iret
