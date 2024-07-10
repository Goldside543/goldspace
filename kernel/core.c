void kernel_main();  // Forward declaration

#include <stddef.h>
#include <stdint.h>
#include "../gash/shell.h"
#include "io.h"
#include "multiboot.h"

multiboot_header_t mb_header = {
    .magic = 0x1BADB002,
    .flags = 0x0,
    .checksum = -(0x1BADB002 + 0x0),
    .header_addr = (uint32_t)&mb_header,
    .load_addr = 0x00100000,
    .load_end_addr = 0x00A00000,
    .bss_end_addr = 0x00B00000,
    .entry_addr = (uint32_t)&kernel_main
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
        size_t command_len = 0;
        print("> ");

        // Read user input
        while (1) {
            char c = get_char();
            if (c == '\n') {
                command[command_len] = '\0';
                break;
            }
            command[command_len++] = c;
            print(&c);  // Echo back the character
        }

        // Execute command
        shell_execute_command(command);
    }
}
