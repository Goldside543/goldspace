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
    ; Set up segment registers
    mov ax, DATA_SEG
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

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
