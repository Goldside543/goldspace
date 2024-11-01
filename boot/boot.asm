; SPDX-License-Identifier: GPL-2.0-only

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

    ; Load segment register ES with value 0x1000
    mov ax, 0x1000    ; Load the segment value into AX
    mov es, ax        ; Transfer the value from AX to ES

    ; Write a value to a specific memory location
    mov di, 0x0000         ; Offset address within the segment
    mov al, 0xAA           ; Value to write
    mov [es:di], al        ; Write value to memory

    ; Read the value back from the same memory location
    mov al, [es:di]        ; Read value from memory

    ; Compare the read value with the original value
    cmp al, 0xAA           ; Compare with expected value
    jne memory_error       ; Jump if not equal (error)

    ; Switch to protected mode
    cli                ; Disable interrupts
    lgdt [gdt_descriptor] ; Load the GDT

    ; Set up protected mode control registers
    mov eax, cr0
    or eax, 1
    mov cr0, eax

    jmp CODE_SEG:init_protected_mode

memory_error:
    ; Set video mode to 320x200 256-color (Mode 13h) for error display
    mov ah, 0x00
    mov al, 0x13
    int 0x10

    ; Load segment register ES with value 0xA000 (Video memory segment)
    mov ax, 0xA000    ; Load the segment value into AX
    mov es, ax        ; Transfer the value from AX to ES

    ; Fill the screen with light green color (color index 2)
    mov cx, 0xFA00      ; Number of pixels (320x200)
    mov al, 0x02        ; Light green color index
    mov di, 0x0000      ; Video memory segment
    rep stosb           ; Fill video memory with color

    ; Halt the system
    cli                 ; Clear interrupts
    hlt                 ; Halt CPU

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
