# SPDX-License-Identifier: GPL-2.0-only

LD = ld -z noexecstack		# Edit this to change the linker
CC = gcc --std=gnu17		# Edit this to change the C compiler
AS = as				# For now, as is the only option
ARCH = -m32			# 64-bit support is lackluster, so only -m32 is gonna work
LD_ARCH = -m elf_i386		# Architecture used in linker
DEBUG = -g			# Just remove -g and debug symbols will be disabled, making a smaller kernel binary
WARNINGS = -Werror		# If you want to see warnings, use -Wall here, I guess
RUSTFLAGS = -A warnings 	# Here because Rust wouldn't shut the frick (keeping it PG-13) up about warnings

.PHONY: clean
.PHONY: all

all: goldspace.iso

goldspace.iso: kernel/kernel.bin
	mkdir -p isodir/boot/grub
	cp kernel/kernel.bin isodir/boot/
	cp grub.cfg isodir/boot/grub/
	grub-mkrescue -o goldspace.iso isodir

kernel/kernel.bin: kernel/kernel.o gash/shell.o kernel/string.o fs/ramfs/ramfs.o mm/memory.o drivers/audio.o drivers/keyboard.o drivers/usb.o drivers/graphics.o drivers/mouse.o drivers/disk.o drivers/gpu.o drivers/rtc.o kernel/window.o kernel/abs.o kernel/cpudelay.o kernel/syscall_dispatcher.o kernel/syscall_table.o kernel/execute.o kernel/process.o ipc/ipc.o kernel/panic.o kernel/idt.o kernel/interrupt.o drivers/vga.o fs/vfs/vfs.o drivers/pci.o security/aslr.o kernel/gdt.o kernel/tss.o kernel/keyboard_isr_wrapper.o kernel/pit_isr_wrapper.o kernel/privileges.o kernel/vm86.o kernel/enter_user_mode.o kernel/ring3.o kernel/software_isr_wrapper.o drivers/serial.o kernel/gpf_isr_wrapper.o
	$(LD) $(DEBUG) $(LD_ARCH) -T kernel/linker.ld -o kernel/kernel.bin kernel/kernel.o gash/shell.o kernel/string.o fs/ramfs/ramfs.o mm/memory.o drivers/audio.o drivers/keyboard.o drivers/usb.o drivers/graphics.o drivers/mouse.o drivers/disk.o drivers/gpu.o drivers/rtc.o kernel/window.o kernel/abs.o kernel/cpudelay.o kernel/syscall_dispatcher.o kernel/syscall_table.o kernel/execute.o kernel/process.o ipc/ipc.o kernel/panic.o kernel/idt.o kernel/interrupt.o drivers/vga.o fs/vfs/vfs.o drivers/pci.o security/aslr.o kernel/gdt.o kernel/tss.o kernel/keyboard_isr_wrapper.o kernel/pit_isr_wrapper.o kernel/privileges.o kernel/vm86.o kernel/enter_user_mode.o kernel/ring3.o kernel/software_isr_wrapper.o drivers/serial.o kernel/gpf_isr_wrapper.o

kernel/kernel.o: kernel/core.c
	$(CC) $(DEBUG) $(ARCH) $(WARNINGS) -ffreestanding -fno-stack-protector -c kernel/core.c -o kernel/kernel.o

ipc/ipc.o: ipc/ipc.c
	$(CC) $(DEBUG) $(ARCH) $(WARNINGS) -ffreestanding -fno-stack-protector -c ipc/ipc.c -o ipc/ipc.o

gash/shell.o: gash/shell.c
	$(CC) $(DEBUG) $(ARCH) $(WARNINGS) -ffreestanding -fno-stack-protector -c gash/shell.c -o gash/shell.o

kernel/string.o: kernel/string.c
	$(CC) $(DEBUG) $(ARCH) $(WARNINGS) -ffreestanding -fno-stack-protector -c kernel/string.c -o kernel/string.o

mm/memory.o: mm/memory.c
	$(CC) $(DEBUG) $(ARCH) $(WARNINGS) -ffreestanding -fstack-protector-strong -c mm/memory.c -o mm/memory.o

drivers/audio.o: drivers/audio.c
	$(CC) $(DEBUG) $(ARCH) $(WARNINGS) -ffreestanding -fno-stack-protector -c drivers/audio.c -o drivers/audio.o

drivers/keyboard.o: drivers/keyboard.c
	$(CC) $(DEBUG) $(ARCH) $(WARNINGS) -ffreestanding -fno-stack-protector -c drivers/keyboard.c -o drivers/keyboard.o

drivers/usb.o: drivers/usb.c
	$(CC) $(DEBUG) $(ARCH) $(WARNINGS) -ffreestanding -fno-stack-protector -c drivers/usb.c -o drivers/usb.o

drivers/graphics.o: drivers/graphics.c
	$(CC) $(DEBUG) $(ARCH) $(WARNINGS) -ffreestanding -fno-stack-protector -c drivers/graphics.c -o drivers/graphics.o

drivers/mouse.o: drivers/mouse.c
	$(CC) $(DEBUG) $(ARCH) $(WARNINGS) -ffreestanding -fno-stack-protector -c drivers/mouse.c -o drivers/mouse.o

drivers/disk.o: drivers/disk.c
	$(CC) $(DEBUG) $(ARCH) $(WARNINGS) -ffreestanding -fno-stack-protector -c drivers/disk.c -o drivers/disk.o

drivers/gpu.o: drivers/gpu.c
	$(CC) $(DEBUG) $(ARCH) $(WARNINGS) -ffreestanding -fno-stack-protector -c drivers/gpu.c -o drivers/gpu.o

drivers/rtc.o: drivers/rtc.c
	$(CC) $(DEBUG) $(ARCH) $(WARNINGS) -ffreestanding -fno-stack-protector -c drivers/rtc.c -o drivers/rtc.o

kernel/window.o: kernel/window.c
	$(CC) $(DEBUG) $(ARCH) $(WARNINGS) -ffreestanding -fno-stack-protector -c kernel/window.c -o kernel/window.o

kernel/abs.o: kernel/abs.c
	$(CC) $(DEBUG) $(ARCH) $(WARNINGS) -ffreestanding -fno-stack-protector -c kernel/abs.c -o kernel/abs.o

kernel/cpudelay.o: kernel/cpudelay.c
	$(CC) $(DEBUG) $(ARCH) $(WARNINGS) -ffreestanding -fno-stack-protector -c kernel/cpudelay.c -o kernel/cpudelay.o

kernel/syscall_dispatcher.o: kernel/syscall_dispatcher.c
	$(CC) $(DEBUG) $(ARCH) $(WARNINGS) -ffreestanding -fno-stack-protector -c kernel/syscall_dispatcher.c -o kernel/syscall_dispatcher.o

kernel/syscall_table.o: kernel/syscall_table.c
	$(CC) $(DEBUG) $(ARCH) $(WARNINGS) -ffreestanding -fno-stack-protector -c kernel/syscall_table.c -o kernel/syscall_table.o

kernel/execute.o: kernel/execute.c
	$(CC) $(DEBUG) $(ARCH) $(WARNINGS) -ffreestanding -fno-stack-protector -c kernel/execute.c -o kernel/execute.o

rust/target/i686-unknown-linux-gnu/release/libgoldspacerust.a: rust/src/lib.rs
	cd rust && cargo build --target i686-unknown-linux-gnu --release

kernel/process.o: kernel/process.c
	$(CC) $(DEBUG) $(ARCH) $(WARNINGS) -ffreestanding -fstack-protector-strong -c kernel/process.c -o kernel/process.o

kernel/panic.o: kernel/panic.c
	$(CC) $(DEBUG) $(ARCH) $(WARNINGS) -ffreestanding -fno-stack-protector -c kernel/panic.c -o kernel/panic.o

kernel/interrupt.o: kernel/interrupt.c
	$(CC) $(DEBUG) $(ARCH) $(WARNINGS) -ffreestanding -fno-stack-protector -c kernel/interrupt.c -o kernel/interrupt.o

kernel/idt.o: kernel/idt.c
	$(CC) $(DEBUG) $(ARCH) $(WARNINGS) -ffreestanding -fno-stack-protector -c kernel/idt.c -o kernel/idt.o

drivers/vga.o: drivers/vga.c
	$(CC) $(DEBUG) $(ARCH) $(WARNINGS) -ffreestanding -fno-stack-protector -c drivers/vga.c -o drivers/vga.o

fs/vfs/vfs.o: fs/vfs/vfs.c
	$(CC) $(DEBUG) $(ARCH) $(WARNINGS) -ffreestanding -fno-stack-protector -c fs/vfs/vfs.c -o fs/vfs/vfs.o

drivers/pci.o: drivers/pci.c
	$(CC) $(DEBUG) $(ARCH) $(WARNINGS) -ffreestanding -fno-stack-protector -c drivers/pci.c -o drivers/pci.o

kernel/gdt.o: kernel/gdt.c
	$(CC) $(DEBUG) $(ARCH) $(WARNINGS) -ffreestanding -fno-stack-protector -c kernel/gdt.c -o kernel/gdt.o

security/aslr.o: security/aslr.c
	$(CC) $(DEBUG) $(ARCH) $(WARNINGS) -ffreestanding -fno-stack-protector -c security/aslr.c -o security/aslr.o

kernel/enter_user_mode.o: kernel/enter_user_mode.c
	$(CC) $(DEBUG) $(ARCH) $(WARNINGS) -ffreestanding -fno-stack-protector -c kernel/enter_user_mode.c -o kernel/enter_user_mode.o

fs/ramfs/ramfs.o: fs/ramfs/ramfs.c
	$(CC) $(DEBUG) $(ARCH) $(WARNINGS) -ffreestanding -fno-stack-protector -c fs/ramfs/ramfs.c -o fs/ramfs/ramfs.o

drivers/serial.o: drivers/serial.c
	$(CC) $(DEBUG) $(ARCH) $(WARNINGS) -ffreestanding -fno-stack-protector -c drivers/serial.c -o drivers/serial.o

kernel/tss.o: kernel/tss.s
	$(AS) -32 -o kernel/tss.o kernel/tss.s

kernel/keyboard_isr_wrapper.o: kernel/keyboard_isr_wrapper.s
	$(AS) -32 -o kernel/keyboard_isr_wrapper.o kernel/keyboard_isr_wrapper.s

kernel/pit_isr_wrapper.o: kernel/pit_isr_wrapper.s
	$(AS) -32 -o kernel/pit_isr_wrapper.o kernel/pit_isr_wrapper.s

kernel/software_isr_wrapper.o: kernel/software_isr_wrapper.s
	$(AS) -32 -o kernel/software_isr_wrapper.o kernel/software_isr_wrapper.s

kernel/privileges.o: kernel/privileges.s
	$(AS) -32 -o kernel/privileges.o kernel/privileges.s

kernel/vm86.o: kernel/vm86.s
	$(AS) -32 -o kernel/vm86.o kernel/vm86.s

kernel/ring3.o: kernel/ring3.s
	$(AS) -32 -o kernel/ring3.o kernel/ring3.s

kernel/gpf_isr_wrapper.o: kernel/gpf_isr_wrapper.s
	$(AS) -32 -o kernel/gpf_isr_wrapper.o kernel/gpf_isr_wrapper.s

clean:
	rm -rf *.bin *.o *.iso isodir rust/target kernel/*.o drivers/*.o net/*.o kernel/kernel.bin fs/*.o mm/*.o ipc/*.o gash/*.o
