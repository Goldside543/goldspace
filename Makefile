all: os.iso

os.iso: isodir/boot/kernel.bin grub.cfg
	mkdir -p isodir/boot/grub
	cp grub.cfg isodir/boot/grub/
	grub-mkrescue -o os.iso isodir

isodir/boot/kernel.bin: kernel/kernel.bin
	mkdir -p isodir/boot
	cp kernel/kernel.bin isodir/boot/

kernel/kernel.bin: boot/boot.bin kernel/kernel.o gash/shell.o kernel/string.o fs/bffs.o
	ld -m elf_i386 -T kernel/linker.ld -o kernel/kernel.bin boot/boot.bin kernel/kernel.o gash/shell.o kernel/string.o fs/bffs.o

boot/boot.bin: boot/boot.asm
	nasm -f bin boot/boot.asm -o boot/boot.bin

kernel/kernel.o: kernel/core.c
	gcc -m32 -ffreestanding -fno-stack-protector -c kernel/core.c -o kernel/kernel.o

gash/shell.o: gash/shell.c
	gcc -m32 -ffreestanding -fno-stack-protector -c gash/shell.c -o gash/shell.o

kernel/string.o: kernel/string.c
	gcc -m32 -ffreestanding -fno-stack-protector -c kernel/string.c -o kernel/string.o

fs/bffs.o: fs/bffs.c
	gcc -m32 -ffreestanding -fno-stack-protector -c fs/bffs.c -o fs/bffs.o

clean:
	rm -rf *.bin *.o *.iso isodir
