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

volatile uint16_t *VideoMemory = (volatile uint16_t *)0xB8000;

// Cursor position variables
static uint8_t cursor_x = 0;
static uint8_t cursor_y = 0;

// Keyboard interrupt handler
void keyboard_handler() {
    // Read scan code from keyboard port (0x60)
    uint8_t scan_code = inb(0x60);

    // Convert scan code to ASCII character using a keymap (simplified example)
    char c = keymap[scan_code];

    // Display character on-screen
    if (c != '\0') {
        display_char(c);
    }

    // Acknowledge interrupt to PIC (for IRQ 1, keyboard)
    outb(0x20, 0x20);
}

// Initialize PIC and install keyboard interrupt handler
void init_keyboard() {
    // Install keyboard interrupt handler (IRQ 1)
    set_irq_handler(1, keyboard_handler);

    // Enable IRQ 1 (keyboard) on PIC
    enable_irq(1);
}

// Initialize VGA text mode cursor
void init_cursor() {
    move_cursor();
}

// Move cursor on-screen
void move_cursor() {
    uint16_t cursorLocation = cursor_y * 80 + cursor_x;
    outb(0x3D4, 14);                  // Tell VGA board we are setting the high cursor byte
    outb(0x3D5, cursorLocation >> 8); // Send the high cursor byte
    outb(0x3D4, 15);                  // Tell VGA board we are setting the low cursor byte
    outb(0x3D5, cursorLocation);      // Send the low cursor byte
}

// Display character on-screen
void display_char(char c) {
    if (c >= ' ' && c <= '~') {  // Printable ASCII characters
        VideoMemory[cursor_y * 80 + cursor_x] = (0x0F << 8) | c;
        cursor_x++;
    }

    if (cursor_x >= 80) {
        cursor_x = 0;
        cursor_y++;
    }
    if (cursor_y >= 25) {
        // Implement scrolling logic here
    }
    move_cursor();
}

// Kernel main function
void kernel_main() {
    // Initialize cursor position
    cursor_x = 0;
    cursor_y = 0;

    // Initialize VGA cursor
    init_cursor();

    // Initialize keyboard
    init_keyboard();

    // Print welcome message
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
