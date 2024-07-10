#include <stddef.h>
#include <stdint.h>

void kernel_main() {
    // print the welcome thingy
    const char *str = "Welcome to Goldspace.";
    unsigned int i = 0;
    char *videomem = (char *)0xb8000;

    while (str[i] != '\0') {
        videomem[i * 2] = str[i];
        videomem[i * 2 + 1] = 0x07; // Light grey on black background
        i++;
    }

    while (1) {}
}
