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

void display_char(char c) {
    if (c >= ' ' && c <= '~') {  // Printable ASCII characters
        VideoMemory[cursor_y * 80 + cursor_x] = (0x0F << 8) | c;
    } else if (c == '\n') {
        cursor_x = 0;
        cursor_y++;
    } else if (c == '\r') {
        cursor_x = 0;
    } else if (c == '\b') {  // Handle backspace
        if (cursor_x > 0) {
            cursor_x--;
            VideoMemory[cursor_y * 80 + cursor_x] = (0x0F << 8) | ' ';
        }
    } else if (c == '\t') {  // Handle tab
        cursor_x = (cursor_x + 8) & ~(8 - 1);  // Tab stop every 8 columns
    } else if (c >= '\x01' && c <= '\x1F') {  // Control characters
        // Handle as needed (e.g., ignore, special representation)
    } else {
        // Handle non-printable or unrecognized characters
        VideoMemory[cursor_y * 80 + cursor_x] = (0x0F << 8) | '?';  // Placeholder for unrecognized characters
    }

    cursor_x++;
    if (cursor_x >= 80) {
        cursor_x = 0;
        cursor_y++;
    }
    if (cursor_y >= 25) {
        // Scroll up logic as before
    }
    move_cursor();
}


    cursor_x++;
    if (cursor_x >= 80) {
        cursor_x = 0;
        cursor_y++;
    }
    if (cursor_y >= 25) {
        for (int i = 0; i < 24 * 80; i++) {
            VideoMemory[i] = VideoMemory[i + 80];
        }
        for (int i = 24 * 80; i < 25 * 80; i++) {
            VideoMemory[i] = (0x0F << 8) | ' ';
        }
        cursor_y = 24;
    }
    move_cursor();
}

void print(const char *str) {
    while (*str != '\0') {
        display_char(*str++);
    }
}

char get_char() {
    // Define key code mappings for special keys
    static const char keymap[] = {
        0, 0, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b', '\t', // 0-15
        'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n', 0, 'a', 's', // 16-31
        'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`', 0, '\\', 'z', 'x', 'c', 'v', // 32-47
        'b', 'n', 'm', ',', '.', '/', 0, '*', 0, ' ', 0, // 48-59
        0, 0, 0, 0, 0, 0, 0, '7', '8', '9', '-', '4', '5', '6', '+', '1', // 60-75
        '2', '3', '0', '.', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 76-91
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 92-107
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 108-123
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 // 124-139
    };

    // Read character from keyboard port (for simplicity, polling)
    while (!(inb(0x64) & 0x01));  // Wait until input buffer is not empty

    char keycode = inb(0x60);    // Read from keyboard port

    if (keycode >= 0 && keycode < sizeof(keymap)) {
        return keymap[keycode];
    }

    return 0;  // Return null for unrecognized key codes
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
                display_char(c);
            }
        }

        // Execute command
        shell_execute_command(command);
    }
}
