all: os.iso

os.iso: boot/boot.bin kernel/kernel.bin
    mkdir -p isodir/boot/grub
    cp boot/boot.bin isodir/boot/
    cp kernel/kernel.bin isodir/boot/
    cp grub.cfg isodir/boot/grub/
    grub-mkrescue -o os.iso isodir

boot/boot.bin: boot/boot.asm
    nasm -f bin boot/boot.asm -o boot/boot.bin

kernel/kernel.bin: kernel/kernel.o gash/shell.o kernel/string.o
    ld -m elf_i386 -T kernel/linker.ld -o kernel/kernel.bin kernel/kernel.o gash/shell.o kernel/string.o --oformat binary

kernel/kernel.o: kernel/kernel.c
    gcc -m32 -ffreestanding -c kernel/core.c -o kernel/kernel.o

gash/shell.o: gash/shell.c
    gcc -m32 -ffreestanding -c gash/shell.c -o gash/shell.o

kernel/string.o: kernel/string.c
    gcc -m32 -ffreestanding -c kernel/string.c -o kernel/string.o

clean:
    rm -rf *.bin *.o *.iso isodir
