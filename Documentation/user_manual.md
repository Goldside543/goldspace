# Goldspace User Manual

## Table of Contents
1. [Introduction](#introduction)
2. [Installation](#installation)
3. [Getting Started](#getting-started)
4. [Using the Gash Shell](#using-the-gash-shell)
5. [Available Commands](#available-commands)
6. [System Features](#system-features)
7. [Troubleshooting](#troubleshooting)

## Introduction

Welcome to Goldspace, a Unix-like kernel written from scratch! This manual will guide you through installing, running, and using Goldspace on your computer.

Goldspace is designed as a learning project that demonstrates core operating system concepts. It features a custom bootloader, kernel, memory management system, and Gash shell for user interaction.

## Installation

### Creating a Bootable USB Drive

To create a bootable USB drive for Goldspace:

1. Download the Goldspace ISO file
2. Use a tool like Rufus (Windows) or dd (Linux) to write the ISO to a USB drive
3. Insert the USB drive into your computer and boot from it

### Burning to CD/DVD

To burn Goldspace to a CD or DVD:

1. Download the Goldspace ISO file
2. Use a disc burning application to write the ISO to a blank disc
3. Boot your computer from the disc

## Getting Started

After booting Goldspace, you'll be greeted by the Gash shell prompt. The system will display some startup information including the version number and copyright notice.

The prompt will look like this:
```
> 
```

You can now start typing commands to interact with the system.

## Using the Gash Shell

The Gash shell is Goldspace's command interpreter. It allows you to execute commands and interact with the kernel.

### Basic Usage

To execute a command, simply type it at the prompt and press Enter. For example:
```
> help
```

Some commands take arguments. For example, to echo a message:
```
> echo Hello, World!
```

### Command History

The shell does not currently support command history. You'll need to retype commands if you want to execute them again.

### Clearing the Screen

If the screen becomes cluttered, you can clear it with the `clear` command:
```
> clear
```

## Available Commands

Here are all the available commands in Gash:

### help
Displays a list of available commands and their descriptions.

Usage:
```
> help
```

### echo
Displays the specified message on the screen.

Usage:
```
> echo <message>
```

Example:
```
> echo Hello, World!
```

### clear
Clears the screen by scrolling down several lines.

Usage:
```
> clear
```

### render
Performs a rendering test on the GPU by drawing a rectangle.

Usage:
```
> render
```

### panic
Triggers a kernel panic for testing purposes. This will crash the system.

Usage:
```
> panic
```

### builddate
Displays the date and time when the kernel was compiled.

Usage:
```
> builddate
```

### testputs
Tests the system call for printing messages.

Usage:
```
> testputs <message>
```

### mode13h
Switches the display to graphics mode 13h (320x200 256-color mode) and draws a rectangle.

Usage:
```
> mode13h
```

### scan
Scans the PCI bus for connected devices.

Usage:
```
> scan
```

### vendor
Displays information about the CPU vendor.

Usage:
```
> vendor
```

### mandel
Renders a Mandelbrot set in the VGA text mode.

Usage:
```
> mandel
```

### calculate
Performs basic arithmetic calculations (+, -, *, /).

Usage:
```
> calculate <number1> <operator> <number2>
```

Example:
```
> calculate 10 + 5
```

### reboot
Reboots the system.

Usage:
```
> reboot
```

## System Features

### Memory Management
Goldspace implements a custom heap allocator with:
- Block splitting and coalescing for efficient memory usage
- Corruption detection with magic numbers
- Support for allocation, deallocation, and reallocation

### Process Management
The kernel supports basic process management:
- Process creation and termination
- Round-robin scheduling
- Context switching between processes

### Device Support
Goldspace includes drivers for:
- PS/2 Keyboard
- VGA Text Mode Display
- Programmable Interval Timer (PIT)
- PCI Bus Scanning
- Graphics Mode 13h

### Security Features
- Address Space Layout Randomization (ASLR) for process memory layout randomization

## Troubleshooting

### Keyboard Not Responding
If the keyboard is not responding correctly:
1. Make sure you're using a PS/2 keyboard or a USB keyboard in PS/2 compatibility mode
2. Restart the system
3. If problems persist, this may be a limitation of the current implementation

### Graphics Issues
If graphics commands are not working:
1. Ensure your virtual machine or computer supports the requested graphics modes
2. Some graphics features may not work in all environments

### System Crashes
If the system crashes or freezes:
1. Try using the `reboot` command if possible
2. If that doesn't work, use your computer's reset button
3. Report the issue to the development team with details about what you were doing

### Reporting Issues
If you encounter bugs or issues with Goldspace:
1. Note the exact steps you took before the issue occurred
2. Record any error messages displayed
3. Report the issue to the project's GitHub repository