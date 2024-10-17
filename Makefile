# SPDX-License-Identifier: GPL-2.0-only

all: goldspace.iso

goldspace.iso: boot/boot.bin kernel/kernel.bin
	mkdir -p isodir/boot/grub
	cp boot/boot.bin isodir/boot/
	cp kernel/kernel.bin isodir/boot/
	cp grub.cfg isodir/boot/grub/
	grub-mkrescue -o goldspace.iso isodir

boot/boot.bin: boot/boot.asm
	nasm -f bin boot/boot.asm -o boot/boot.bin

kernel/kernel.bin: kernel/kernel.o rust/target/i686-unknown-linux-gnu/release/libgoldspacerust.a gash/shell.o kernel/string.o fs/bffs.o net/net-io.o drivers/rtl8139.o net/sockets.o net/arp.o mm/memory.o drivers/audio.o drivers/keyboard.o drivers/usb.o drivers/graphics.o drivers/mouse.o drivers/disk.o drivers/gpu.o drivers/rtc.o kernel/window.o kernel/abs.o kernel/cpudelay.o kernel/syscall_dispatcher.o kernel/syscall_table.o fs/fs_syscalls.o kernel/execute.o kernel/process.o ipc/ipc.o
	ld -g -m elf_i386 -T kernel/linker.ld -o kernel/kernel.bin kernel/kernel.o gash/shell.o kernel/string.o fs/bffs.o net/net-io.o drivers/rtl8139.o net/sockets.o net/arp.o mm/memory.o drivers/audio.o drivers/keyboard.o drivers/usb.o drivers/graphics.o drivers/mouse.o drivers/disk.o drivers/gpu.o drivers/rtc.o kernel/window.o kernel/abs.o kernel/cpudelay.o kernel/syscall_dispatcher.o kernel/syscall_table.o fs/fs_syscalls.o kernel/execute.o kernel/process.o rust/target/i686-unknown-linux-gnu/release/libgoldspacerust.a ipc/ipc.o

kernel/kernel.o: kernel/core.c
	gcc -g -m32 -ffreestanding -fno-stack-protector -c kernel/core.c -o kernel/kernel.o

ipc/ipc.o: ipc/ipc.c
	gcc -g -m32 -ffreestanding -fno-stack-protector -c ipc/ipc.c -o ipc/ipc.o

gash/shell.o: gash/shell.c
	gcc -g -m32 -ffreestanding -fno-stack-protector -c gash/shell.c -o gash/shell.o

kernel/string.o: kernel/string.c
	gcc -g -m32 -ffreestanding -fno-stack-protector -c kernel/string.c -o kernel/string.o

fs/bffs.o: fs/bffs.c
	gcc -g -m32 -ffreestanding -fno-stack-protector -c fs/bffs.c -o fs/bffs.o

net/net-io.o: net/net-io.c
	gcc -g -m32 -ffreestanding -fno-stack-protector -c net/net-io.c -o net/net-io.o

drivers/rtl8139.o: drivers/rtl8139.c
	gcc -g -m32 -ffreestanding -fno-stack-protector -c drivers/rtl8139.c -o drivers/rtl8139.o

net/sockets.o: net/sockets.c
	gcc -g -m32 -ffreestanding -fno-stack-protector -c net/sockets.c -o net/sockets.o

net/arp.o: net/arp.c
	gcc -g -m32 -ffreestanding -fno-stack-protector -c net/arp.c -o net/arp.o

mm/memory.o: mm/memory.c
	gcc -g -m32 -ffreestanding -fstack-protector-strong -c mm/memory.c -o mm/memory.o

drivers/audio.o: drivers/audio.c
	gcc -g -m32 -ffreestanding -fno-stack-protector -c drivers/audio.c -o drivers/audio.o

drivers/keyboard.o: drivers/keyboard.c
	gcc -g -m32 -ffreestanding -fno-stack-protector -c drivers/keyboard.c -o drivers/keyboard.o

drivers/usb.o: drivers/usb.c
	gcc -g -m32 -ffreestanding -fno-stack-protector -c drivers/usb.c -o drivers/usb.o

drivers/graphics.o: drivers/graphics.c
	gcc -g -m32 -ffreestanding -fno-stack-protector -c drivers/graphics.c -o drivers/graphics.o

drivers/mouse.o: drivers/mouse.c
	gcc -g -m32 -ffreestanding -fno-stack-protector -c drivers/mouse.c -o drivers/mouse.o

drivers/disk.o: drivers/disk.c
	gcc -g -m32 -ffreestanding -fno-stack-protector -c drivers/disk.c -o drivers/disk.o

drivers/gpu.o: drivers/gpu.c
	gcc -g -m32 -ffreestanding -fno-stack-protector -c drivers/gpu.c -o drivers/gpu.o

drivers/rtc.o: drivers/rtc.c
	gcc -g -m32 -ffreestanding -fno-stack-protector -c drivers/rtc.c -o drivers/rtc.o

kernel/window.o: kernel/window.c
	gcc -g -m32 -ffreestanding -fno-stack-protector -c kernel/window.c -o kernel/window.o

kernel/abs.o: kernel/abs.c
	gcc -g -m32 -ffreestanding -fno-stack-protector -c kernel/abs.c -o kernel/abs.o

kernel/cpudelay.o: kernel/cpudelay.c
	gcc -g -m32 -ffreestanding -fno-stack-protector -c kernel/cpudelay.c -o kernel/cpudelay.o

kernel/syscall_dispatcher.o: kernel/syscall_dispatcher.c
	gcc -g -m32 -ffreestanding -fno-stack-protector -c kernel/syscall_dispatcher.c -o kernel/syscall_dispatcher.o

kernel/syscall_table.o: kernel/syscall_table.c
	gcc -g -m32 -ffreestanding -fno-stack-protector -c kernel/syscall_table.c -o kernel/syscall_table.o

fs/fs_syscalls.o: fs/fs_syscalls.c
	gcc -g -m32 -ffreestanding -fno-stack-protector -c fs/fs_syscalls.c -o fs/fs_syscalls.o

kernel/execute.o: kernel/execute.c
	gcc -g -m32 -ffreestanding -fstack-protector-strong -c kernel/execute.c -o kernel/execute.o

rust/target/i686-unknown-linux-gnu/release/libgoldspacerust.a: rust/src/lib.rs
	cd rust && cargo build --target i686-unknown-linux-gnu --release

kernel/process.o: kernel/process.c
	gcc -g -m32 -ffreestanding -fstack-protector-strong -c kernel/process.c -o kernel/process.o

clean:
	rm -rf *.bin *.o *.iso isodir rust/target kernel/*.o drivers/*.o net/*.o kernel/kernel.bin boot/boot.bin fs/*.o mm/*.o ipc/*.o gash/*.o
