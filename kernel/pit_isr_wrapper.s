.global pit_isr_wrapper

pit_isr_wrapper:
    pushal
    cld              # C code following the sysV ABI requires DF to be clear on function entry
    call pit_isr
    popal
    iret
