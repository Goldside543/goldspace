#include <stddef.h>
#include <stdint.h>

void print(const char *str) {
    unsigned short *VideoMemory = (unsigned short *)0xb8000;
    for (int i = 0; str[i] != '\0'; i++) {
        VideoMemory[i] = (VideoMemory[i] & 0xFF00) | str[i];
    }
}

void kernel_main() {
    print("Hello, Kernel World!");
    while (1) {}
}
