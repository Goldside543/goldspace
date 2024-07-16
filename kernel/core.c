void kernel_main();  // Forward declaration

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "../gash/shell.h"
#include "io.h"
#include "multiboot.h"

multiboot_header_t mb_header = {
    .magic = 0x1BADB002,
    .flags = 0x0,
    .checksum = -(0x1BADB002 + 0x0),
    .header_addr = (uint32_t)&mb_header,
    .load_addr = 0x0,
    .load_end_addr = 0x0,
    .bss_end_addr = 0x0,
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
            case '\b':  // Handle backspace
                if (cursor_x > 0) {
                    cursor_x--;
                    VideoMemory[cursor_y * 80 + cursor_x] = (VideoMemory[cursor_y * 80 + cursor_x] & 0xFF00) | ' ';
                }
                break;
            default:
                // Write character to VGA buffer at current cursor position
                VideoMemory[cursor_y * 80 + cursor_x] = (VideoMemory[cursor_y * 80 + cursor_x] & 0xFF00) | *str;
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
                VideoMemory[i] = (VideoMemory[i] & 0xFF00) | ' ';
            }
            cursor_y = 24;
        }

        str++;
    }
    move_cursor();
}

void print_char(char c) {
    char str[2] = {c, '\0'};
    print(str);
}

// Improved scancode to ASCII table
char scancode_to_ascii_table[128] = {
    0, 0, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b', '\t',  // '\b' is backspace, '\t' is tab
    'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n', 0,  // '\n' is enter
    'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`', 0,
    '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0, '*',
    0, ' ', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '7',
    '8', '9', '-', '4', '5', '6', '+', '1', '2', '3', '0', '.', 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

char input_buffer[256];
int input_len = 0;

char get_char() {
    static bool extended = false;
    uint8_t scancode;

    // Read character from keyboard port (for simplicity, polling)
    while (!(inb(0x64) & 0x01));  // Wait until input buffer is not empty
    scancode = inb(0x60);  // Get the scan code

    if (scancode == 0xE0) {  // If it's the first byte of a multi-byte scan code
        extended = true;  // Set a flag to indicate we're in the middle of a multi-byte scan code
        return 0;
    }

    if (scancode & 0x80) {  // If it's a key release event
        return 0;  // Ignore it
    }

    if (extended) {  // If we're in the middle of a multi-byte scan code
        extended = false;  // Reset the flag
        // Handle the second byte of the scan code (you'll need to add cases for each arrow key)
        switch (scancode) {
            case 0x48: return 'U';  // Up arrow
            case 0x50: return 'D';  // Down arrow
            case 0x4B: return 'L';  // Left arrow
            case 0x4D: return 'R';  // Right arrow
            default: return 0;
        }
    } else {
        // Convert scan code to ASCII
        char ascii = scancode_to_ascii_table[scancode];
        
        // Handle special characters
        if (ascii == '\b') {  // If backspace was pressed
            if (input_len > 0) {  // If there are characters in the buffer
                input_len--;  // Remove the last character
                input_buffer[input_len] = '\0';  // Null-terminate the string
            }
        } else if (ascii == '\r' || ascii == '\n') {  // If newline or carriage return
            // End of input line, null-terminate and reset input_len
            input_buffer[input_len] = '\0';  // Null-terminate the string
            input_len = 0;  // Reset input length for the next input
        } else if (ascii != 0 && input_len < sizeof(input_buffer) - 1) {  // If a regular character was pressed and there's room in the buffer
            input_buffer[input_len] = ascii;  // Add the character to the buffer
            input_len++;  // Increment the length
            input_buffer[input_len] = '\0';  // Null-terminate the string
        }
        
        return ascii;
    }
}

void kernel_main() {
    // Initialize cursor position
    cursor_x = 0;
    cursor_y = 0;
    move_cursor();

    print("Welcome to Goldspace and the Gash shell!\n");
    print("Type 'h' for available commands.\n");

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
