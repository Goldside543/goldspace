# SPDX-License-Identifier: GPL-2.0-only

LD = ld
CC = gcc
AS = nasm
ARCH = -m32

.PHONY: clean

all: goldspace.iso

goldspace.iso: boot/boot.bin kernel/kernel.bin
	mkdir -p isodir/boot/grub
	cp boot/boot.bin isodir/boot/
	cp kernel/kernel.bin isodir/boot/
	cp grub.cfg isodir/boot/grub/
	grub-mkrescue -o goldspace.iso isodir

boot/boot.bin: boot/boot.asm
	$(AS) -f bin boot/boot.asm -o boot/boot.bin

kernel/kernel.bin: kernel/kernel.o rust/target/i686-unknown-linux-gnu/release/libgoldspacerust.a gash/shell.o kernel/string.o fs/bffs.o net/net-io.o drivers/rtl8139.o net/sockets.o net/arp.o mm/memory.o drivers/audio.o drivers/keyboard.o drivers/usb.o drivers/graphics.o drivers/mouse.o drivers/disk.o drivers/gpu.o drivers/rtc.o kernel/window.o kernel/abs.o kernel/cpudelay.o kernel/syscall_dispatcher.o kernel/syscall_table.o fs/fs_syscalls.o kernel/execute.o kernel/process.o ipc/ipc.o kernel/panic.o
	$(LD) -g -m elf_i386 -T kernel/linker.ld -o kernel/kernel.bin kernel/kernel.o gash/shell.o kernel/string.o fs/bffs.o net/net-io.o drivers/rtl8139.o net/sockets.o net/arp.o mm/memory.o drivers/audio.o drivers/keyboard.o drivers/usb.o drivers/graphics.o drivers/mouse.o drivers/disk.o drivers/gpu.o drivers/rtc.o kernel/window.o kernel/abs.o kernel/cpudelay.o kernel/syscall_dispatcher.o kernel/syscall_table.o fs/fs_syscalls.o kernel/execute.o kernel/process.o rust/target/i686-unknown-linux-gnu/release/libgoldspacerust.a ipc/ipc.o kernel/panic.o

kernel/kernel.o: kernel/core.c
	$(CC) -g $(ARCH) -ffreestanding -fno-stack-protector -c kernel/core.c -o kernel/kernel.o

ipc/ipc.o: ipc/ipc.c
	$(CC) -g $(ARCH) -ffreestanding -fno-stack-protector -c ipc/ipc.c -o ipc/ipc.o

gash/shell.o: gash/shell.c
	$(CC) -g $(ARCH) -ffreestanding -fno-stack-protector -c gash/shell.c -o gash/shell.o

kernel/string.o: kernel/string.c
	$(CC) -g $(ARCH) -ffreestanding -fno-stack-protector -c kernel/string.c -o kernel/string.o

fs/bffs.o: fs/bffs.c
	$(CC) -g $(ARCH) -ffreestanding -fno-stack-protector -c fs/bffs.c -o fs/bffs.o

net/net-io.o: net/net-io.c
	$(CC) -g $(ARCH) -ffreestanding -fno-stack-protector -c net/net-io.c -o net/net-io.o

drivers/rtl8139.o: drivers/rtl8139.c
	$(CC) -g $(ARCH) -ffreestanding -fno-stack-protector -c drivers/rtl8139.c -o drivers/rtl8139.o

net/sockets.o: net/sockets.c
	$(CC) -g $(ARCH) -ffreestanding -fno-stack-protector -c net/sockets.c -o net/sockets.o

net/arp.o: net/arp.c
	$(CC) -g $(ARCH) -ffreestanding -fno-stack-protector -c net/arp.c -o net/arp.o

mm/memory.o: mm/memory.c
	$(CC) -g $(ARCH) -ffreestanding -fstack-protector-strong -c mm/memory.c -o mm/memory.o

drivers/audio.o: drivers/audio.c
	$(CC) -g $(ARCH) -ffreestanding -fno-stack-protector -c drivers/audio.c -o drivers/audio.o

drivers/keyboard.o: drivers/keyboard.c
	$(CC) -g $(ARCH) -ffreestanding -fno-stack-protector -c drivers/keyboard.c -o drivers/keyboard.o

drivers/usb.o: drivers/usb.c
	$(CC) -g $(ARCH) -ffreestanding -fno-stack-protector -c drivers/usb.c -o drivers/usb.o

drivers/graphics.o: drivers/graphics.c
	$(CC) -g $(ARCH) -ffreestanding -fno-stack-protector -c drivers/graphics.c -o drivers/graphics.o

drivers/mouse.o: drivers/mouse.c
	$(CC) -g $(ARCH) -ffreestanding -fno-stack-protector -c drivers/mouse.c -o drivers/mouse.o

drivers/disk.o: drivers/disk.c
	$(CC) -g $(ARCH) -ffreestanding -fno-stack-protector -c drivers/disk.c -o drivers/disk.o

drivers/gpu.o: drivers/gpu.c
	$(CC) -g $(ARCH) -ffreestanding -fno-stack-protector -c drivers/gpu.c -o drivers/gpu.o

drivers/rtc.o: drivers/rtc.c
	$(CC) -g $(ARCH) -ffreestanding -fno-stack-protector -c drivers/rtc.c -o drivers/rtc.o

kernel/window.o: kernel/window.c
	$(CC) -g $(ARCH) -ffreestanding -fno-stack-protector -c kernel/window.c -o kernel/window.o

kernel/abs.o: kernel/abs.c
	$(CC) -g $(ARCH) -ffreestanding -fno-stack-protector -c kernel/abs.c -o kernel/abs.o

kernel/cpudelay.o: kernel/cpudelay.c
	$(CC) -g $(ARCH) -ffreestanding -fno-stack-protector -c kernel/cpudelay.c -o kernel/cpudelay.o

kernel/syscall_dispatcher.o: kernel/syscall_dispatcher.c
	$(CC) -g $(ARCH) -ffreestanding -fno-stack-protector -c kernel/syscall_dispatcher.c -o kernel/syscall_dispatcher.o

kernel/syscall_table.o: kernel/syscall_table.c
	$(CC) -g $(ARCH) -ffreestanding -fno-stack-protector -c kernel/syscall_table.c -o kernel/syscall_table.o

fs/fs_syscalls.o: fs/fs_syscalls.c
	$(CC) -g $(ARCH) -ffreestanding -fno-stack-protector -c fs/fs_syscalls.c -o fs/fs_syscalls.o

kernel/execute.o: kernel/execute.c
	$(CC) -g $(ARCH) -ffreestanding -fstack-protector-strong -c kernel/execute.c -o kernel/execute.o

rust/target/i686-unknown-linux-gnu/release/libgoldspacerust.a: rust/src/lib.rs
	cd rust && cargo build --target i686-unknown-linux-gnu --release

kernel/process.o: kernel/process.c
	$(CC) -g $(ARCH) -ffreestanding -fstack-protector-strong -c kernel/process.c -o kernel/process.o

kernel/panic.o: kernel/panic.c
	$(CC) -g $(ARCH) -ffreestanding -fno-stack-protector -c kernel/panic.c -o kernel/panic.o

clean:
	rm -rf *.bin *.o *.iso isodir rust/target kernel/*.o drivers/*.o net/*.o kernel/kernel.bin boot/boot.bin fs/*.o mm/*.o ipc/*.o gash/*.o
