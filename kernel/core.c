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

// Define VGA text mode buffer address
volatile uint16_t *VideoMemory = (volatile uint16_t *)0xB8000;

// Cursor position variables
static uint8_t cursor_x = 0;
static uint8_t cursor_y = 0;

void move_cursor() {
    uint16_t cursorLocation = cursor_y * 80 + cursor_x;
    outb(0x3D4, 14);                  // Tell VGA board we are setting the high cursor byte
    outb(0x3D5, cursorLocation >> 8); // Send the high cursor byte
    outb(0x3D4, 15);                  // Tell VGA board we are setting the low cursor byte
    outb(0x3D5, cursorLocation);      // Send the low cursor byte
}

void print(const char *str) {
    while (*str != '\0') {
        switch (*str) {
            case '\n':
                cursor_x = 0;
                cursor_y++;
                break;
            default:
                // Write character to VGA buffer at current cursor position with white on black color
                VideoMemory[cursor_y * 80 + cursor_x] = (0x0F << 8) | *str;
                cursor_x++;
                break;
        }

        // Check if we need to wrap to next line
        if (cursor_x >= 80) {
            cursor_x = 0;
            cursor_y++;
        }

        // Check if we need to scroll
        if (cursor_y >= 25) {
            // Scroll up by copying each line up one row
            for (int i = 0; i < 24 * 80; i++) {
                VideoMemory[i] = VideoMemory[i + 80];
            }
            // Clear the last line
            for (int i = 24 * 80; i < 25 * 80; i++) {
                VideoMemory[i] = (0x0F << 8) | ' ';
            }
            cursor_y = 24;
        }

        str++;
    }
    move_cursor();
}

void print_char(char c) {
    // Write character to VGA buffer at current cursor position with white on black color
    switch (c) {
        case '\n':
            cursor_x = 0;
            cursor_y++;
            break;
        default:
            VideoMemory[cursor_y * 80 + cursor_x] = (0x0F << 8) | c;
            cursor_x++;
            break;
    }

    // Check if we need to wrap to next line
    if (cursor_x >= 80) {
        cursor_x = 0;
        cursor_y++;
    }

    // Check if we need to scroll
    if (cursor_y >= 25) {
        // Scroll up by copying each line up one row
        for (int i = 0; i < 24 * 80; i++) {
            VideoMemory[i] = VideoMemory[i + 80];
        }
        // Clear the last line
        for (int i = 24 * 80; i < 25 * 80; i++) {
            VideoMemory[i] = (0x0F << 8) | ' ';
        }
        cursor_y = 24;
    }

    move_cursor();
}

char get_char() {
    // Read character from keyboard port (for simplicity, polling)
    while (!(inb(0x64) & 0x01));  // Wait until input buffer is not empty
    return inb(0x60);             // Read from keyboard port
}

void kernel_main() {
    // Initialize cursor position
    cursor_x = 0;
    cursor_y = 0;
    move_cursor();

    print("Welcome to Goldspace and the Gash shell!\n");
    print("Type 'help' for available commands.\n");

    while (1) {
        char command[256];
        size_t command_len = 0;

        // Read user input
        print("> ");
        while (1) {
            char c = get_char();
            if (c == '\n' || c == '\r') {
                command[command_len] = '\0';  // Null-terminate the command string
                break;
            } else {
                command[command_len++] = c;
                // Echo back the character to the screen
                print_char(c);
            }
        }

        // Execute command
        shell_execute_command(command);
    }
}
