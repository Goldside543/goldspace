.global keyboard_isr_wrapper

keyboard_isr_wrapper:
    pushal
    cld              # C code following the sysV ABI requires DF to be clear on function entry
    call keyboard_isr
    popal
    iret
