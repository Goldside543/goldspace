all: goldspace.iso

goldspace.iso: boot/boot.bin kernel/kernel.bin
	mkdir -p isodir/boot/grub
	cp boot/boot.bin isodir/boot/
	cp kernel/kernel.bin isodir/boot/
	cp grub.cfg isodir/boot/grub/
	grub-mkrescue -o goldspace.iso isodir

boot/boot.bin: boot/boot.asm
	nasm -f bin boot/boot.asm -o boot/boot.bin

kernel/kernel.bin: kernel/kernel.o gash/shell.o kernel/string.o fs/bffs.o net/net-io.o net/net_if.o net/sockets.o mm/memory.o drivers/audio.o drivers/keyboard.o drivers/usb.o drivers/graphics.o drivers/mouse.o kernel/window.o kernel/abs.o kernel/cpudelay.o
	ld -m elf_i386 -T kernel/linker.ld -o kernel/kernel.bin kernel/kernel.o gash/shell.o kernel/string.o fs/bffs.o net/net-io.o net/net_if.o net/sockets.o mm/memory.o drivers/audio.o drivers/keyboard.o drivers/usb.o drivers/graphics.o drivers/mouse.o kernel/window.o kernel/abs.o kernel/cpudelay.o

kernel/kernel.o: kernel/core.c
	gcc -m32 -ffreestanding -fno-stack-protector -c kernel/core.c -o kernel/kernel.o

gash/shell.o: gash/shell.c
	gcc -m32 -ffreestanding -fno-stack-protector -c gash/shell.c -o gash/shell.o

kernel/string.o: kernel/string.c
	gcc -m32 -ffreestanding -fno-stack-protector -c kernel/string.c -o kernel/string.o

fs/bffs.o: fs/bffs.c
	gcc -m32 -ffreestanding -fno-stack-protector -c fs/bffs.c -o fs/bffs.o

net/net-io.o: net/net-io.c
	gcc -m32 -ffreestanding -fno-stack-protector -c net/net-io.c -o net/net-io.o

net/net_if.o: net/net_if.c
	gcc -m32 -ffreestanding -fno-stack-protector -c net/net_if.c -o net/net_if.o

net/sockets.o: net/sockets.c
	gcc -m32 -ffreestanding -fno-stack-protector -c net/sockets.c -o net/sockets.o

mm/memory.o: mm/memory.c
	gcc -m32 -ffreestanding -fno-stack-protector -c mm/memory.c -o mm/memory.o

drivers/audio.o: drivers/audio.c
	gcc -m32 -ffreestanding -fno-stack-protector -c drivers/audio.c -o drivers/audio.o

drivers/keyboard.o: drivers/keyboard.c
	gcc -m32 -ffreestanding -fno-stack-protector -c drivers/keyboard.c -o drivers/keyboard.o

drivers/usb.o: drivers/usb.c
	gcc -m32 -ffreestanding -fno-stack-protector -c drivers/usb.c -o drivers/usb.o

drivers/graphics.o: drivers/graphics.c
	gcc -m32 -ffreestanding -fno-stack-protector -c drivers/graphics.c -o drivers/graphics.o

drivers/mouse.o: drivers/mouse.c
	gcc -m32 -ffreestanding -fno-stack-protector -c drivers/mouse.c -o drivers/mouse.o

kernel/window.o: kernel/window.c
	gcc -m32 -ffreestanding -fno-stack-protector -c kernel/window.c -o kernel/window.o

kernel/abs.o: kernel/abs.c
	gcc -m32 -ffreestanding -fno-stack-protector -c kernel/abs.c -o kernel/abs.o

kernel/cpudelay.o: kernel/cpudelay.c
	gcc -m32 -ffreestanding -fno-stack-protector -c kernel/cpudelay.c -o kernel/cpudelay.o

clean:
	rm -rf *.bin *.o *.iso isodir
