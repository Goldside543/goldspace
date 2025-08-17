# Goldspace Technical Specification

## System Overview

Goldspace is a Unix-like kernel implemented from scratch, designed as an educational project to demonstrate core operating system concepts. It features a custom bootloader, kernel, memory management system, and basic shell interface.

## Architecture

### Target Platform
- Architecture: x86 (32-bit)
- Processor Mode: Protected mode
- Memory Model: Flat memory model

### System Components

1. **Bootloader**
   - Initializes system hardware
   - Loads kernel into memory
   - Transfers control to kernel

2. **Kernel Core**
   - Global Descriptor Table (GDT) setup
   - Interrupt Descriptor Table (IDT) setup
   - Memory management initialization
   - Device driver initialization
   - Main execution loop

3. **Memory Management**
   - Heap allocator (first-fit algorithm)
   - Virtual memory with paging
   - Memory protection mechanisms

4. **Interrupt Handling**
   - Hardware interrupt processing
   - Exception handling
   - System call interface

5. **Process Management**
   - Process control blocks
   - Task scheduling
   - Context switching

6. **Device Drivers**
   - PS/2 Keyboard
   - VGA Text Mode Display
   - Programmable Interval Timer (PIT)
   - Serial Communication
   - Graphics Mode 13h
   - PCI Bus

7. **File System**
   - Virtual File System (VFS) layer
   - RAM-based file system (RAMFS)

8. **Security Features**
   - Address Space Layout Randomization (ASLR)
   - Hardware random number generation support

## Memory Layout

### Physical Memory Map
```
0x00000000 - 0x00100000: Reserved/BIOS area
0x00100000 - 0x00200000: Kernel code and data
0x00200000 - 0x00300000: Heap (1MB)
0x00300000+ : Available for allocation
0x000B8000: VGA text buffer
```

### Virtual Memory
- Identity mapping for entire 4GB address space
- 4KB page size
- Two-level page table structure

## Interrupt Handling

### IDT Configuration
- 256 entries
- Software interrupt (0x80) for system calls
- Hardware interrupts for PIT (0x20) and keyboard (0x21)
- Exception handlers for GPF (0x0D) and Double Fault (0x08)

### IRQ Mapping
- Master PIC: IRQs 0-7 mapped to interrupts 0x20-0x27
- Slave PIC: IRQs 8-15 mapped to interrupts 0x28-0x2F

### Interrupt Handlers
1. **System Call Handler**
   - Entry point: interrupt 0x80
   - Dispatches to syscall_table based on EAX value
   - Returns result in EAX

2. **Keyboard Handler**
   - Triggered by IRQ1
   - Reads scancodes from port 0x60
   - Converts scancodes to ASCII
   - Updates input buffer

3. **PIT Handler**
   - Triggered by IRQ0 at 250Hz
   - Updates system time
   - Triggers process scheduling

## Process Management

### Process Control Block (PCB)
```c
typedef struct pcb {
    int pid;                // Process ID
    uint32_t esp;           // Stack pointer
    uint32_t ebp;           // Base pointer
    uint32_t eip;           // Instruction pointer
    uint32_t *page_directory; // Page directory address
    uint32_t *stack;        // Stack address
    int state;              // Process state
    struct pcb *next;       // Next process in queue
} pcb_t;
```

### Process States
- PROCESS_RUNNING: Currently executing
- PROCESS_READY: Ready to execute
- PROCESS_BLOCKED: Blocked waiting for resource
- PROCESS_TERMINATED: Terminated

### Scheduling Algorithm
- Round-robin scheduling
- Time slice: Based on PIT interrupts
- Context switching on each schedule call

## System Calls

### Available System Calls
| Call Number | Name       | Function           | Arguments                          |
|-------------|------------|--------------------|------------------------------------|
| 0           | SYS_OPEN   | Open a file        | const char *pathname, int flags    |
| 1           | SYS_WRITE  | Write to file      | int fd, const void *buf, size_t count |
| 2           | SYS_READ   | Read from file     | int fd, void *buf, size_t count    |
| 3           | SYS_CLOSE  | Close file         | int fd                             |
| 4           | SYS_EXECV  | Execute program    | const char *pathname, char *const argv[] |
| 5           | SYS_YIELD  | Yield processor    | void                               |
| 6           | SYS_EXIT   | Terminate process  | void                               |
| 7           | SYS_STAT   | Get file status    | const char *pathname, struct stat *statbuf |
| 8           | SYS_TESTPUTS | Print string     | const char *str                    |

### System Call Interface
1. Load system call number into EAX
2. Load arguments into EBX, ECX, EDX, ESI (as needed)
3. Invoke interrupt 0x80
4. Result returned in EAX

## Device Drivers

### PS/2 Keyboard
- Scancode processing
- ASCII conversion
- Special key handling (shift, control, etc.)
- Buffer management

### VGA Text Mode
- 80x25 character display
- 16 foreground colors
- Cursor management
- Scrolling support

### Programmable Interval Timer (PIT)
- Channel 0 configured for rate generation
- 250 Hz interrupt rate
- System time tracking

### Serial Port
- COM1 (0x3F8) initialization
- Basic output functionality
- Debug logging support

### Graphics Mode 13h
- 320x200 resolution
- 256-color palette
- Basic drawing primitives

### PCI Bus
- Configuration space access
- Device enumeration
- Vendor/device identification

## File System

### Virtual File System (VFS)
- Abstraction layer for file operations
- Standard file operations (open, read, write, close)
- File status information

### RAM-based File System (RAMFS)
- Memory-resident file storage
- Basic file operations
- Simple directory structure

## Security Features

### Address Space Layout Randomization (ASLR)
- Randomizes process memory layout
- Uses hardware random number generation (RDRAND)
- Falls back to TSC-based entropy if RDRAND unavailable

### Memory Protection
- Heap corruption detection with magic numbers
- Boundary checking in allocator
- Null pointer dereference prevention

## Build System

### Compiler Requirements
- GCC (GNU Compiler Collection)
- GNU Assembler (GAS)
- GNU Linker (LD)

### Build Process
1. Compile C source files to object files
2. Assemble assembly source files
3. Link all object files with linker script
4. Generate bootable binary
5. Create ISO image with GRUB bootloader

### Makefile Targets
- `all`: Complete build process
- `clean`: Remove build artifacts
- `goldspace.iso`: Create bootable ISO

## Testing and Debugging

### Emulation
- QEMU system emulator
- Support for both i386 and x86_64 targets

### Debugging Features
- Serial port output for logging
- Built-in print functions
- Kernel panic mechanism for error reporting

### Testing Methodology
- Manual testing in QEMU
- Verification of basic functionality
- Stress testing of memory allocator
- Interrupt handling validation

## Known Limitations

1. **Memory Management**
   - Fixed-size heap (1MB)
   - No virtual memory swapping
   - Limited memory protection

2. **Process Management**
   - No inter-process communication (IPC)
   - Basic scheduling only
   - No process priorities

3. **Device Support**
   - Limited hardware support
   - No USB support
   - No network stack

4. **File System**
   - RAM-based only
   - No persistent storage
   - Basic file operations only

5. **Security**
   - Basic ASLR implementation
   - No user authentication
   - Limited access controls

## Future Enhancements

1. **Memory Management**
   - Slab allocator implementation
   - Virtual memory swapping
   - Enhanced memory protection

2. **Process Management**
   - Inter-process communication (IPC)
   - Process priorities and scheduling algorithms
   - Process groups and sessions

3. **Device Support**
   - USB device support
   - Network stack implementation
   - Advanced graphics support

4. **File System**
   - Persistent file systems
   - Multiple file system support
   - Advanced file operations

5. **Security**
   - User management and authentication
   - Access control mechanisms
   - Encryption support

6. **System Services**
   - Background services/daemons
   - System logging facility
   - Configuration management