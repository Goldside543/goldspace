# SPDX-License-Identifier: GPL-2.0-only

LD = ld 	# Edit this to change the linker
CC = gcc	# Edit this to change the C compiler
AS = nasm	# For now, nasm is the only option
ARCH = -m32	# 64-bit support is lackluster, so only -m32 is gonna work
DEBUG = -g 	# Just remove -g and debug symbols will be disabled, making a smaller kernel binary

.PHONY: clean

all: goldspace.iso

goldspace.iso: boot/boot.bin kernel/kernel.bin
	mkdir -p isodir/boot/grub
	cp kernel/kernel.bin isodir/boot/
	cp grub.cfg isodir/boot/grub/
	grub-mkrescue -o goldspace.iso isodir

boot/boot.bin: boot/boot.asm
	$(AS) -f bin boot/boot.asm -o boot/boot.bin

kernel/kernel.bin: kernel/kernel.o rust/target/i686-unknown-linux-gnu/release/libgoldspacerust.a gash/shell.o kernel/string.o fs/bffs/bffs.o net/net-io.o drivers/rtl8139.o net/sockets.o net/arp.o mm/memory.o drivers/audio.o drivers/keyboard.o drivers/usb.o drivers/graphics.o drivers/mouse.o drivers/disk.o drivers/gpu.o drivers/rtc.o kernel/window.o kernel/abs.o kernel/cpudelay.o kernel/syscall_dispatcher.o kernel/syscall_table.o fs/fs_syscalls.o kernel/execute.o kernel/process.o ipc/ipc.o kernel/panic.o kernel/v86.o kernel/idt.o kernel/interrupt.o drivers/vga.o fs/vfs/vfs.c
	$(LD) $(DEBUG) -m elf_i386 -T kernel/linker.ld -o kernel/kernel.bin kernel/kernel.o gash/shell.o kernel/string.o fs/bffs/bffs.o net/net-io.o drivers/rtl8139.o net/sockets.o net/arp.o mm/memory.o drivers/audio.o drivers/keyboard.o drivers/usb.o drivers/graphics.o drivers/mouse.o drivers/disk.o drivers/gpu.o drivers/rtc.o kernel/window.o kernel/abs.o kernel/cpudelay.o kernel/syscall_dispatcher.o kernel/syscall_table.o fs/fs_syscalls.o kernel/execute.o kernel/process.o rust/target/i686-unknown-linux-gnu/release/libgoldspacerust.a ipc/ipc.o kernel/panic.o kernel/v86.o kernel/idt.o kernel/interrupt.o drivers/vga.o fs/vfs/vfs.c

kernel/kernel.o: kernel/core.c
	$(CC) $(DEBUG) $(ARCH) -ffreestanding -fno-stack-protector -c kernel/core.c -o kernel/kernel.o

ipc/ipc.o: ipc/ipc.c
	$(CC) $(DEBUG) $(ARCH) -ffreestanding -fno-stack-protector -c ipc/ipc.c -o ipc/ipc.o

gash/shell.o: gash/shell.c
	$(CC) $(DEBUG) $(ARCH) -ffreestanding -fno-stack-protector -c gash/shell.c -o gash/shell.o

kernel/string.o: kernel/string.c
	$(CC) $(DEBUG) $(ARCH) -ffreestanding -fno-stack-protector -c kernel/string.c -o kernel/string.o

fs/bffs/bffs.o: fs/bffs/bffs.c
	$(CC) $(DEBUG) $(ARCH) -ffreestanding -fno-stack-protector -c fs/bffs.c -o fs/bffs.o

net/net-io.o: net/net-io.c
	$(CC) $(DEBUG) $(ARCH) -ffreestanding -fno-stack-protector -c net/net-io.c -o net/net-io.o

drivers/rtl8139.o: drivers/rtl8139.c
	$(CC) $(DEBUG) $(ARCH) -ffreestanding -fno-stack-protector -c drivers/rtl8139.c -o drivers/rtl8139.o

net/sockets.o: net/sockets.c
	$(CC) $(DEBUG) $(ARCH) -ffreestanding -fno-stack-protector -c net/sockets.c -o net/sockets.o

net/arp.o: net/arp.c
	$(CC) $(DEBUG) $(ARCH) -ffreestanding -fno-stack-protector -c net/arp.c -o net/arp.o

mm/memory.o: mm/memory.c
	$(CC) $(DEBUG) $(ARCH) -ffreestanding -fstack-protector-strong -c mm/memory.c -o mm/memory.o

drivers/audio.o: drivers/audio.c
	$(CC) $(DEBUG) $(ARCH) -ffreestanding -fno-stack-protector -c drivers/audio.c -o drivers/audio.o

drivers/keyboard.o: drivers/keyboard.c
	$(CC) $(DEBUG) $(ARCH) -ffreestanding -fno-stack-protector -c drivers/keyboard.c -o drivers/keyboard.o

drivers/usb.o: drivers/usb.c
	$(CC) $(DEBUG) $(ARCH) -ffreestanding -fno-stack-protector -c drivers/usb.c -o drivers/usb.o

drivers/graphics.o: drivers/graphics.c
	$(CC) $(DEBUG) $(ARCH) -ffreestanding -fno-stack-protector -c drivers/graphics.c -o drivers/graphics.o

drivers/mouse.o: drivers/mouse.c
	$(CC) $(DEBUG) $(ARCH) -ffreestanding -fno-stack-protector -c drivers/mouse.c -o drivers/mouse.o

drivers/disk.o: drivers/disk.c
	$(CC) $(DEBUG) $(ARCH) -ffreestanding -fno-stack-protector -c drivers/disk.c -o drivers/disk.o

drivers/gpu.o: drivers/gpu.c
	$(CC) $(DEBUG) $(ARCH) -ffreestanding -fno-stack-protector -c drivers/gpu.c -o drivers/gpu.o

drivers/rtc.o: drivers/rtc.c
	$(CC) $(DEBUG) $(ARCH) -ffreestanding -fno-stack-protector -c drivers/rtc.c -o drivers/rtc.o

kernel/window.o: kernel/window.c
	$(CC) $(DEBUG) $(ARCH) -ffreestanding -fno-stack-protector -c kernel/window.c -o kernel/window.o

kernel/abs.o: kernel/abs.c
	$(CC) $(DEBUG) $(ARCH) -ffreestanding -fno-stack-protector -c kernel/abs.c -o kernel/abs.o

kernel/cpudelay.o: kernel/cpudelay.c
	$(CC) $(DEBUG) $(ARCH) -ffreestanding -fno-stack-protector -c kernel/cpudelay.c -o kernel/cpudelay.o

kernel/syscall_dispatcher.o: kernel/syscall_dispatcher.c
	$(CC) $(DEBUG) $(ARCH) -ffreestanding -fno-stack-protector -c kernel/syscall_dispatcher.c -o kernel/syscall_dispatcher.o

kernel/syscall_table.o: kernel/syscall_table.c
	$(CC) $(DEBUG) $(ARCH) -ffreestanding -fno-stack-protector -c kernel/syscall_table.c -o kernel/syscall_table.o

fs/fs_syscalls.o: fs/fs_syscalls.c
	$(CC) $(DEBUG) $(ARCH) -ffreestanding -fno-stack-protector -c fs/fs_syscalls.c -o fs/fs_syscalls.o

kernel/execute.o: kernel/execute.c
	$(CC) $(DEBUG) $(ARCH) -ffreestanding -fstack-protector-strong -c kernel/execute.c -o kernel/execute.o

rust/target/i686-unknown-linux-gnu/release/libgoldspacerust.a: rust/src/lib.rs
	cd rust && cargo build --target i686-unknown-linux-gnu --release

kernel/process.o: kernel/process.c
	$(CC) $(DEBUG) $(ARCH) -ffreestanding -fstack-protector-strong -c kernel/process.c -o kernel/process.o

kernel/panic.o: kernel/panic.c
	$(CC) $(DEBUG) $(ARCH) -ffreestanding -fno-stack-protector -c kernel/panic.c -o kernel/panic.o

kernel/v86.o: kernel/v86.c
	$(CC) $(DEBUG) $(ARCH) -ffreestanding -fno-stack-protector -c kernel/v86.c -o kernel/v86.o

kernel/interrupt.o: kernel/interrupt.c
	$(CC) $(DEBUG) $(ARCH) -ffreestanding -fno-stack-protector -c kernel/interrupt.c -o kernel/interrupt.o

kernel/idt.o: kernel/idt.c
	$(CC) $(DEBUG) $(ARCH) -ffreestanding -fno-stack-protector -c kernel/idt.c -o kernel/idt.o

drivers/vga.o: drivers/vga.c
	$(CC) $(DEBUG) $(ARCH) -ffreestanding -fno-stack-protector -c drivers/vga.c -o drivers/vga.o

fs/vfs/vfs.o: fs/vfs/vfs.c
	$(CC) $(DEBUG) $(ARCH) -ffreestanding -fno-stack-protector -c fs/vfs/vfs.c -o fs/vfs/vfs.o

clean:
	rm -rf *.bin *.o *.iso isodir rust/target kernel/*.o drivers/*.o net/*.o kernel/kernel.bin boot/boot.bin fs/*.o mm/*.o ipc/*.o gash/*.o
