![Goldspace Logo](https://github.com/user-attachments/assets/341a14bd-a23a-4742-ac67-bb30a29c6892)

# Goldspace

Goldspace is a Unix-like kernel written from scratch, featuring a bootloader, kernel, linker script, Gash shell, and build system. It's a learning project that demonstrates core operating system concepts.

## Features

- Custom bootloader
- Memory management with heap allocation
- Paging support for virtual memory
- Interrupt handling (IDT)
- Task scheduling and process management
- Gash shell for user interaction
- Basic device drivers (keyboard, timer, VGA)
- System call interface
- Graphics support
- Serial communication
- PCI device enumeration
- Address Space Layout Randomization (ASLR)

## Architecture

Goldspace currently supports x86 architecture with plans to expand to x86_64. The kernel implements:

- Protected mode operation
- Flat memory model
- Preemptive multitasking
- Virtual memory with paging
- Hardware abstraction layer

## Gash Shell

Gash (Goldspace Shell) is the built-in command interpreter. It provides a simple interface for interacting with the kernel and executing commands.

## Compilation Instructions

### Prerequisites

Before building Goldspace, ensure you have the following tools installed:
- GCC (GNU Compiler Collection)
- GNU Make
- GNU Linker (ld)
- GNU Assembler (as)
- GRUB tools (grub-mkrescue)
- QEMU (for testing)

On Ubuntu/Debian:
```bash
sudo apt-get install build-essential nasm grub-pc-bin grub-common xorriso qemu-system-x86
```

On Fedora:
```bash
sudo dnf install gcc make nasm grub2 xorriso qemu-system-x86
```

### 32-bit Version

```bash
git clone https://github.com/Goldside543/goldspace.git
cd goldspace
make -j$(nproc)
```

### 64-bit Version *(Experimental)*

```bash
git clone https://github.com/Goldside543/goldspace.git
cd goldspace
chmod +x scripts/64-bitifier.sh
sed -i "s|path/to/kernel/source|$(pwd)|" scripts/64-bitifier.sh
scripts/64-bitifier.sh
make -j$(nproc)
```

## Running Goldspace

After compilation, you can run Goldspace using QEMU:

```bash
qemu-system-i386 -kernel goldspace.bin
```

Or for the 64-bit version:

```bash
qemu-system-x86_64 -kernel goldspace64.bin
```

Alternatively, you can create and run a bootable ISO:

```bash
make goldspace.iso
qemu-system-i386 -cdrom goldspace.iso
```

## Command List

- `help` - Display available commands
- `clear` - Clear the screen
- `echo <message>` - Display a message
- `render` - Perform a rendering test on the GPU
- `panic` - Trigger a kernel panic (for testing)
- `builddate` - Print build date and time
- `testputs <message>` - Test system call for printing
- `mode13h` - Switch to graphics mode 13h
- `scan` - Scan PCI bus for devices
- `vendor` - Display CPU vendor information
- `mandel` - Render a Mandelbrot set
- `calculate <num1> <op> <num2>` - Perform basic arithmetic
- `reboot` - Reboot the system

## Memory Management

Goldspace implements a custom heap allocator and virtual memory system:

- Heap allocation with kmalloc/kfree
- Paging support with virtual-to-physical address translation
- Memory protection and isolation between processes
- Memory corruption detection with magic numbers

## Interrupt Handling

The kernel implements a complete interrupt handling system:

- IDT (Interrupt Descriptor Table) setup
- Exception handling (GPF, Double Fault, etc.)
- Hardware interrupt handling (PIT, Keyboard)
- Software interrupts for system calls

## Process Management

Goldspace supports basic process management:

- Process creation and termination
- Task scheduling with round-robin algorithm
- Context switching between processes
- Ring 3 user mode execution
- Address Space Layout Randomization (ASLR)

## Known Issues

- Some keyboard input may not register correctly
- Memory management is still being overhauled
- Graphics support is limited
- File system support is minimal

## Documentation

Comprehensive documentation can be found in the `Documentation` directory:

- [User Manual](Documentation/user_manual.md) - For end users of Goldspace
- [Developer Manual](Documentation/developer_manual.md) - For developers contributing to Goldspace
- [Technical Specification](Documentation/technical_specification.md) - Detailed technical information

## Contributing

Contributions are welcome! Please fork the repository and submit a pull request.

1. Fork the repository
2. Create a feature branch (`git checkout -b feature/AmazingFeature`)
3. Commit your changes (`git commit -m 'Add some AmazingFeature'`)
4. Push to the branch (`git push origin feature/AmazingFeature`)
5. Open a pull request

### Development Guidelines

- Follow the existing code style and formatting
- Write clear, descriptive commit messages
- Test your changes thoroughly before submitting
- Update documentation when adding new features
- Keep pull requests focused on a single feature or bug fix

## Donate

If you find this project interesting and would like to support its development, donations are appreciated:

* BTC: bc1qn9zjpgvl63j0dtm29hzkhfe6vxul7nn3w34xek
* LTC: ltc1q524te5cj87ylffcq43jwx7rw8r4l8yl3nrqyc3
* DOGE: DSsFPmvePCAAUJyt6u1Smwz8tJLSrs4nz4
* BCH: qrw04k2gt8gkggqg6sgu3s3y436nlawapgv4t4h87g
* XMR: 44JqnEYZcCQViedo82qWP2Cp2H9jkfatWKdGPfdBK3Lybe46XEdqMVgNTKWwLAhDHDTR7dtum3hUNRRMd2SPZfxME7m8hYL

## Version Information

Current Version: Beta 1.2.0

Goldspace is in the Beta stage with plans for a full release once program support and extensive hardware support are implemented.

## License

Goldspace is licensed under the GPL-2.0 license. See the [LICENSE](LICENSE) file for details.