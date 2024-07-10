#include <stddef.h>
#include <stdint.h>
#include "../gash/shell.h"
#include "io.h"
#include "multiboot.h"

multiboot_header_t mb_header = {
    .magic = 0x1BADB002,
    .flags = 0x0,
    .checksum = -(0x1BADB002 + 0x0),  // Verify the checksum
    .header_addr = (uint32_t)&mb_header,
    .load_addr = 0x00100000,  // Kernel loaded at 0x00100000 in virtual memory
    .load_end_addr = 0x00A00000,  // Arbitrary end address for kernel
    .bss_end_addr = 0x00B00000,  // Arbitrary end address for BSS
    .entry_addr = (uint32_t)&kernel_main  // Entry point of kernel
};

void print(const char *str) {
    unsigned short *VideoMemory = (unsigned short *)0xb8000;
    for (size_t i = 0; str[i] != '\0'; i++) {
        VideoMemory[i] = (VideoMemory[i] & 0xFF00) | str[i];
    }
}

char get_char() {
    // Read character from keyboard port (for simplicity, polling)
    while (!(inb(0x64) & 0x01));  // Wait until input buffer is not empty
    return inb(0x60);             // Read from keyboard port
}

void kernel_main() {
    print("Welcome to Goldspace and the Gash shell!\n");
    print("Type 'help' for available commands.\n");

    while (1) {
        char command[256];
        print("> ");

        // Read user input
        for (size_t i = 0; i < sizeof(command) - 1; ++i) {
            command[i] = get_char();
            command[i + 1] = '\0';  // Ensure null-terminated string
            if (command[i] == '\n') {
                command[i] = '\0';  // Replace newline with null terminator
                break;
            }
            print(command + i);
        }

        // Execute command
        shell_execute_command(command);
    }
}
