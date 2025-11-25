# Goldspace Developer Documentation

## Table of Contents

1. [Architecture Overview](#architecture-overview)
2. [System Components](#system-components)
3. [Memory Management](#memory-management)
4. [Paging Implementation](#paging-implementation)
5. [Interrupt Handling](#interrupt-handling)
6. [Process Management](#process-management)
7. [System Calls](#system-calls)
8. [Device Drivers](#device-drivers)
9. [Build System](#build-system)
10. [Debugging](#debugging)
11. [Contributing](#contributing)

## Architecture Overview

Goldspace is a monolithic kernel designed following Unix principles. The kernel is divided into several subsystems that work together to provide a complete operating system environment.

### Design Principles

- **Simplicity**: The kernel is designed to be as simple as possible while still providing essential functionality
- **Modularity**: Components are separated into distinct modules for easier maintenance and development
- **Educational Value**: The code is written to be readable and understandable for OS development learners
- **Standards Compliance**: Where applicable, the system follows established standards and conventions

### Target Architecture

Goldspace currently targets x86 architecture in 32-bit mode. There are plans to expand support to x86_64 in future versions.

### Memory Model

The kernel uses a flat memory model with the following layout:

```
0x00000000 - 0x00100000: Reserved/BIOS
0x00100000 - 0x00200000: Kernel code and data
0x00200000 - 0x00300000: Heap (1MB)
0x00300000+ : Available for allocation
0x000B8000: VGA text buffer
```

## System Components

### Bootloader
The bootloader is responsible for:
- Initializing the processor to a known state
- Setting up basic hardware
- Loading the kernel into memory
- Transferring control to the kernel

### Kernel Core
The main kernel initialization and control flow:
- GDT and IDT setup
- Memory management initialization
- Device driver initialization
- Starting the Gash shell
- Main execution loop

### Gash Shell
The built-in command interpreter provides:
- User interface for system interaction
- Command parsing and execution
- Built-in utilities for system testing

### Memory Management
Responsible for:
- Heap allocation and deallocation
- Virtual memory management
- Paging implementation
- Memory protection

### Interrupt Handling
Manages:
- Hardware interrupt processing
- Exception handling
- System call interface
- Interrupt descriptor table (IDT) setup

### Process Management
Handles:
- Process creation and termination
- Task scheduling
- Context switching
- Process state management

## Memory Management

### Heap Allocator

Goldspace implements a first-fit heap allocator with the following features:

- Block header/footer with magic numbers for corruption detection
- Block splitting and coalescing for efficient memory usage
- Alignment to 8-byte boundaries
- Support for kmalloc, kfree, and krealloc

#### Data Structures

```c
typedef struct block_header {
    uint32_t magic_head;    // Magic number for corruption detection
    size_t size;            // Size of the block
    int free;               // Flag indicating if block is free
    struct block_header* next;  // Pointer to next block in the list
} block_header;

typedef struct block_footer {
    size_t size;            // Size of the block (duplicate for verification)
    uint32_t magic_tail;    // Magic number for corruption detection
} block_footer;
```

#### Key Functions

- `init_heap()`: Initialize the heap
- `kmalloc(size_t size)`: Allocate memory
- `kfree(void* ptr)`: Free memory
- `krealloc(void* ptr, size_t new_size)`: Resize allocation

#### Memory Pool

The heap uses a fixed-size memory pool defined as:
```c
#define MEMORY_POOL_SIZE (1024 * 1024)  // 1MB pool
static unsigned char memory_pool[MEMORY_POOL_SIZE];
```

### Memory Protection

The system implements basic memory protection through:
- Magic numbers in block headers/footers
- Boundary checking during allocation/deallocation
- Null pointer checks

## Paging Implementation

### Overview

Goldspace implements two-level x86 paging with 4KB pages. The implementation includes:
- Identity mapping for the entire 4GB address space
- Special handling for critical memory regions (VGA buffer)
- Virtual-to-physical address translation

### Key Components

#### Assembly Implementation (kernel/paging.s)

- `init_paging()`: Sets up identity mapping for 4GB
- `enable_paging()`: Enables paging by setting CR0.PG bit

#### C Implementation (mm/memory.c)

- `page_table_init()`: Initializes kernel page table structures
- `map_page()`: Maps virtual to physical addresses
- `kmempaging()`: Handles memory paging for virtual addresses

### Usage

```c
// Initialize paging
init_paging();
page_table_init();

// Enable paging
enable_paging();
```

### Page Directory and Table Structure

The implementation uses standard x86 page directory and page table structures:
- Page directory: 1024 entries, each pointing to a page table
- Page tables: 1024 entries, each pointing to a 4KB page frame

### Address Translation

Virtual addresses are translated to physical addresses using:
1. Top 10 bits index the page directory
2. Middle 10 bits index the page table
3. Bottom 12 bits are the offset within the page

## Interrupt Handling

### IDT Setup

The Interrupt Descriptor Table (IDT) is configured with handlers for:

- Software interrupts (system calls)
- Hardware interrupts (PIT, keyboard)
- Exceptions (GPF, Double Fault)

### Key Functions

- `init_idt()`: Initialize and load the IDT
- `set_idt_entry()`: Set an IDT entry for interrupts
- `set_idt_entry_exception()`: Set an IDT entry for exceptions
- `set_idt_entry_syscall()`: Set an IDT entry for system calls

### Interrupt Service Routines

ISRs are implemented in assembly and follow this pattern:

1. Save registers
2. Call C handler
3. Restore registers
4. Return with `iret`

### IRQ Management

The system uses the standard PC PIC configuration:
- Master PIC: IRQs 0-7 mapped to interrupts 0x20-0x27
- Slave PIC: IRQs 8-15 mapped to interrupts 0x28-0x2F

### Hardware Interrupts

#### PIT (Programmable Interval Timer)
- Default frequency: 250 Hz
- Used for system time tracking
- Triggers process scheduling

#### Keyboard Controller
- PS/2 keyboard support
- Scancode processing
- ASCII conversion

## Process Management

### Process Structure

Processes are represented by the `pcb_t` structure containing:

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

- `PROCESS_RUNNING`: Currently executing
- `PROCESS_READY`: Ready to execute
- `PROCESS_BLOCKED`: Blocked waiting for resource
- `PROCESS_TERMINATED`: Terminated

### Scheduling

Goldspace implements a simple round-robin scheduler:

- Time-slice based scheduling
- Context switching between processes
- Support for process creation and termination

### Key Functions

- `initialize_process_system()`: Initialize process management
- `create_process()`: Create a new process
- `terminate_process()`: Terminate a process
- `schedule()`: Perform context switching
- `context_switch()`: Switch between process contexts

### Context Switching

Context switching involves:
1. Saving the current process's register state
2. Loading the next process's register state
3. Jumping to the next process's instruction pointer

## System Calls

### Implementation

System calls are implemented using interrupt 0x80:

1. User code invokes interrupt 0x80 with system call number in EAX
2. ISR wrapper saves registers
3. `software_interrupt_handler()` dispatches to appropriate handler
4. `syscall_handler()` processes the request
5. Return value placed in EAX
6. ISR wrapper restores registers and returns

### Available System Calls

- `SYS_OPEN`: Open a file
- `SYS_WRITE`: Write to a file descriptor
- `SYS_READ`: Read from a file descriptor
- `SYS_CLOSE`: Close a file descriptor
- `SYS_EXECV`: Execute a program
- `SYS_YIELD`: Yield processor to next process
- `SYS_EXIT`: Terminate current process
- `SYS_STAT`: Get file status
- `SYS_TESTPUTS`: Print a string to console (testing)

### System Call Table

System calls are dispatched through a function pointer table:

```c
int (*syscall_table[])(void*, void*, void*, void*) = {
    [SYS_OPEN]     = (int (*)(void*, void*, void*, void*))vfs_open,
    [SYS_WRITE]    = (int (*)(void*, void*, void*, void*))vfs_write,
    [SYS_READ]     = (int (*)(void*, void*, void*, void*))vfs_read,
    [SYS_CLOSE]    = (int (*)(void*, void*, void*, void*))vfs_close,
    [SYS_EXECV]    = (int (*)(void*, void*, void*, void*))sys_execv,
    [SYS_YIELD]    = (int (*)(void*, void*, void*, void*))sys_yield,
    [SYS_EXIT]     = (int (*)(void*, void*, void*, void*))sys_exit,
    [SYS_STAT]     = (int (*)(void*, void*, void*, void*))vfs_stat,
    [SYS_TESTPUTS] = (int (*)(void*, void*, void*, void*))sys_testputs,
};
```

### Adding New System Calls

To add a new system call:

1. Add a definition to `syscall_numbers.h`
2. Implement the handler function
3. Add an entry to `syscall_table`
4. Update `SYSCALL_TABLE_SIZE` in `syscall_dispatcher.c`

## Device Drivers

### Keyboard Driver

- PS/2 keyboard support
- Scancode to ASCII conversion
- Special key handling (shift, backspace, enter)
- Extended scancode support (arrow keys)

### PIT Driver

- Programmable Interval Timer configuration
- System tick generation (250 Hz)
- Time tracking with Unix timestamp
- Process scheduling integration

### VGA Driver

- Text mode output (80x25 characters)
- Color support
- Cursor management
- Scrolling implementation

### Graphics Driver

- Mode 13h support (320x200 256-color)
- Basic drawing primitives
- Rectangle drawing

### Serial Driver

- Serial port communication
- Debug output support
- Basic UART configuration

### PCI Driver

- PCI bus scanning
- Device enumeration
- Vendor/device ID detection
- Specific device support (RTL8139)

## Build System

### Makefile Structure

The build system uses a single Makefile approach:

- Top-level Makefile coordinates the entire build process
- Object files compiled separately for each source file
- Automatic dependency management through linker script

### Build Targets

- `all`: Build kernel and create ISO
- `clean`: Remove build artifacts
- `goldspace.iso`: Build bootable ISO image

### Configuration

Build configuration is controlled through:

- Makefile variables for compiler and linker options
- Conditional compilation flags
- Architecture-specific settings

### Compiler Settings

The kernel is compiled with specific flags for kernel development:
- `-ffreestanding`: Freestanding environment
- `-fno-stack-protector`: Disable stack protection
- `-m32`: 32-bit target
- `-Wall`: Enable warnings

### Linker Script

The linker script defines:
- Memory layout
- Section placement
- Entry point
- Symbol definitions

## Debugging

### Debugging Tools

- QEMU for emulation and debugging
- GDB for source-level debugging
- Serial output for logging
- Built-in print functions for tracing

### Common Issues

#### VGA Buffer Corruption

When enabling interrupts with paging, the VGA buffer may be corrupted due to:

1. Incorrect virtual-to-physical mapping
2. Interrupt handler accessing unmapped memory
3. Stack overflow corrupting memory

**Solution**: Ensure proper mapping of 0xB8000 in page tables and verify interrupt handlers.

#### Memory Allocation Failures

Heap corruption or exhaustion can cause allocation failures.

**Solution**: Enable memory debugging, check for buffer overruns, and verify coalescing logic.

#### Keyboard Input Issues

Some keyboard inputs may not register correctly due to:

1. Interrupt handling race conditions
2. Buffer overflow
3. Scancode processing errors

**Solution**: Review interrupt handling and buffer management code.

### Debugging Tips

1. Use `print()` statements liberally for tracing
2. Check magic numbers in heap blocks for corruption
3. Verify stack usage in interrupt handlers
4. Use QEMU's monitor for hardware-level debugging
5. Test changes incrementally to isolate issues

### Debugging with QEMU

To run Goldspace in QEMU with debugging enabled:
```bash
qemu-system-i386 -kernel goldspace.bin -s -S
```

Then connect with GDB:
```bash
gdb
(gdb) target remote localhost:1234
(gdb) symbol-file kernel/kernel.bin
```

## Contributing

### Code Style

- Follow existing code formatting conventions
- Use descriptive variable and function names
- Comment complex algorithms and data structures
- Maintain consistent indentation (4 spaces)

### Testing

Before submitting changes:
1. Ensure all existing functionality still works
2. Test in QEMU
3. Verify no new warnings or errors
4. Document any API changes

### Pull Request Process

1. Fork the repository
2. Create a feature branch
3. Make your changes
4. Test thoroughly
5. Submit a pull request with a clear description

### Areas for Improvement

1. Enhanced memory management (slab allocator)
2. File system support
3. Networking stack
4. Advanced scheduling algorithms
5. Multiprocessing support
6. Enhanced security features
7. Better error handling and recovery
8. More comprehensive device driver support