# Goldspace Kernel Documentation

## Overview

Goldspace is a Unix-like kernel with a focus on simplicity and educational value. This documentation provides an overview of the kernel's architecture and key components.

## Memory Management

The kernel implements a custom memory allocator and paging system:

### Heap Management
- Uses a linked list of free and allocated blocks
- Supports kmalloc, kfree, and krealloc functions
- Includes memory corruption detection with magic numbers

### Paging System
- Implements 4KB page size
- Provides identity mapping for the first 4MB of memory
- Ensures proper mapping of critical regions like the VGA framebuffer

## Interrupt Handling

The kernel uses an Interrupt Descriptor Table (IDT) to handle hardware and software interrupts:

- Software interrupts for system calls (INT 0x80)
- Hardware interrupts for keyboard (IRQ1) and timer (IRQ0)
- Exception handlers for critical errors

## Device Drivers

### VGA Driver
- Supports both text mode (0xB8000) and graphics mode
- Provides basic drawing functions

### Keyboard Driver
- Handles PS/2 keyboard input
- Supports extended scancodes for special keys

## System Calls

The kernel provides a basic system call interface through software interrupts.

## Building and Running

See the main README.md for build instructions.