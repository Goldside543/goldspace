.global flush_tss

flush_tss:
    mov $0x28, %ax  # Move the value 0x28 into the ax register
    ltr %ax         # Load the TSS selector (in ax) into the TSS register
    ret             # Return from the function
