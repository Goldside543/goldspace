BITS 16
org 0x7c00

section .multiboot
align 4
    dd 0x1BADB002            ; magic number
    dd 0x00                  ; flags
    dd - (0x1BADB002 + 0x00) ; checksum, which must be magic + flags + checksum = 0

start:
    ; Set up 16-bit real mode segment registers
    xor ax, ax
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov sp, 0x7c00

    ; Load the kernel into memory
    mov bx, 0x1000     ; Address to load the kernel
    mov ah, 0x02       ; BIOS read sector function
    mov al, 10         ; Number of sectors to read
    mov ch, 0          ; Cylinder
    mov cl, 2          ; Sector (starting from 2)
    mov dh, 0          ; Head
    int 0x13

    ; Switch to protected mode
    cli                ; Disable interrupts
    lgdt [gdt_descriptor] ; Load the GDT

    ; Set up protected mode control registers
    mov eax, cr0
    or eax, 1
    mov cr0, eax

    ; Jump to protected mode code
    jmp CODE_SEG:init_protected_mode

gdt_start:
    ; GDT (Global Descriptor Table)
    dq 0x0000000000000000 ; Null descriptor
    dq 0x00cf9a000000ffff ; Code segment descriptor
    dq 0x00cf92000000ffff ; Data segment descriptor

gdt_descriptor:
    dw gdt_end - gdt_start - 1
    dd gdt_start

gdt_end:

init_protected_mode:
    ; Set up 64-bit mode
    mov eax, cr4
    or eax, 0x00000020   ; Enable PAE (Physical Address Extension)
    mov cr4, eax

    ; Set up the page tables for 64-bit mode
    ; This is typically done in more detail, but for simplicity:
    ; Load the PML4 table, PDPT, PD, and PT

    ; Enable 64-bit mode
    mov eax, cr0
    or eax, 0x80000000   ; Enable long mode
    mov cr0, eax

    ; Enable paging
    mov eax, cr3          ; Set up paging
    mov cr3, eax

    ; Jump to 64-bit mode
    jmp 0x08:long_mode_start

long_mode_start:
    ; Set up segment registers for 64-bit mode
    mov eax, 0x10
    mov ds, eax
    mov es, eax
    mov fs, eax
    mov gs, eax
    mov ss, eax

    ; Call the kernel main function
    call KERNEL_OFFSET

    ; Halt the system if kernel_main returns
    hlt

; Define segment selectors
CODE_SEG equ 0x08
DATA_SEG equ 0x10
KERNEL_OFFSET equ 0x1000

times 510-($-$$) db 0
dw 0xAA55