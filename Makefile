all: os.iso

os.iso: boot/boot.bin kernel/kernel.bin
    mkdir -p isodir/boot/grub
    cp boot/boot.bin isodir/boot/
    cp kernel/kernel.bin isodir/boot/
    cp grub.cfg isodir/boot/grub/
    grub-mkrescue -o os.iso isodir

boot/boot.bin: boot/boot.asm
    nasm -f bin boot/boot.asm -o boot/boot.bin

kernel/kernel.bin: kernel/core.c
    gcc -m32 -ffreestanding -c kernel/kernel.c -o kernel/kernel.o
    ld -m elf_i386 -T kernel/linker.ld kernel/kernel.o -o kernel/kernel.bin

clean:
    rm -rf *.bin *.o *.iso isodir
