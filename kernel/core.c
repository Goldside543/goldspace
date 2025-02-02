// SPDX-License-Identifier: GPL-2.0-only
/*
 * kernel/core.c
 *
 * This file is the main part of the kernel, where it all comes together.
 *
 * Copyright (C) 2024-2025 Goldside543
 *
 */

#include <stdbool.h>
void kernel_main();  // Forward declaration
static bool use_keyboard_driver = false;  // This will be set in usb_init()

#include <stddef.h>
#include <stdint.h>
#include "../gash/shell.h"
#include "io.h"
#include "multiboot.h"
#include "../fs/bffs/bffs.h"
#include "../drivers/rtl8139.h"
#include "../drivers/audio.h"
#include "../drivers/usb.h"
#include "../drivers/keyboard.h"
#include "cpudelay.h"
#include "../drivers/graphics.h"
#include "../drivers/mouse.h"
#include "../mm/memory.h"
#include "../drivers/gpu.h"
#include "process.h"
#include "../fs/fat32/fat32.h"
#include "../fs/fs.h"
#include "idt.h"
#include "gdt.h"
#include "../security/aslr.h"

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
volatile uint16_t *VideoMemory = (volatile uint16_t *)0xB8000; // 0xB8000 can be a Problem

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

void irq_set_mask(uint8_t IRQline) {
    uint16_t port;
    uint8_t value;

    if(IRQline < 8) {
        port = 0x21;
    } else {
        port = 0xA1;
        IRQline -= 8;
    }
    value = inb(port) | (1 << IRQline);
    outb(port, value);        
}

char keyboard_isr() {
    if ((inb(0x64) & 1) == 0) {
        return 0;
    }

    uint8_t scancode = inb(0x60);
    static bool extended = false;
    char ascii = 0;

    if (scancode == 0xE0) {  // If it's the first byte of a multi-byte scan code
        extended = true;
        return 0;
    }

    if (scancode & 0x80) {  // If it's a key release event
        extended = false;  // Reset the extended flag
        return 0;
    }

    if (extended) {  // Handle extended scan codes
        extended = false;
        switch (scancode) {
            case 0x48: return 'U';  // Up arrow
            case 0x50: return 'D';  // Down arrow
            case 0x4B: return 'L';  // Left arrow
            case 0x4D: return 'R';  // Right arrow
            default: return 0;
        }
    } else {
        // Convert scan code to ASCII
        ascii = scancode_to_ascii_table[scancode];

        // Handle special characters
        if (ascii == '\b') {  // Backspace
            if (input_len > 0) {
                input_len--;
                input_buffer[input_len] = '\0';
            }
        } else if (ascii == '\r' || ascii == '\n') {  // Enter
            input_buffer[input_len] = '\0';
            input_len = 0;
        } else if (ascii != 0 && input_len < sizeof(input_buffer) - 1) {  // Regular character
            input_buffer[input_len] = ascii;
            input_len++;
            input_buffer[input_len] = '\0';
        }
    }
}

char get_char() {
    if (input_len > 0) {
        char c = input_buffer[0];
	return c;
    }

    return 0;
}

void setup_pit(uint16_t divisor) {
    // Send the control byte to PIT to configure channel 0
    outb(0x43, 0x36); // 0x36 is for channel 0, mode 3 (rate generator), binary counting

    // Send the low byte of the divisor
    outb(0x40, (uint8_t)(divisor & 0xFF));

    // Send the high byte of the divisor
    outb(0x40, (uint8_t)((divisor >> 8) & 0xFF));
}

void kernel_main() {

/* There is an issue with the way the kernel handles input that causes
   some inputs to not register. Anyone who can help, please help ASAP.

            Goldside543
*/
    // Initialize cursor position
    cursor_x = 0;
    cursor_y = 0;
    move_cursor();

    gdt_init();

    setup_pit(4773);

    #if BFFS == 0 
        fs_init();
    #elif BFFS == 1
        fat32_mount();
    #endif

    init_idt();

    page_table_init();

    audio_init();

    usb_init();

    init_keyboard();

    rtl8139_init();

    initialize_process_system();

    // gpu_init();

    init_graphics();

    init_random_seed();

    static unsigned int io_base;

    unsigned char mac_addr[6];
    for (int i = 0; i < 6; i++) {
    mac_addr[i] = inb(io_base + i);  // Read MAC address byte by byte
    }

    net_interface_t iface;
    unsigned int ip = 0xC0A895E2; // IP address 192.168.149.226
    unsigned int netmask = 0xFFFFF000; // Example netmask (255.255.240.0)
    init_net_interface(&iface, "eth0", mac_addr, ip, netmask);

    cpu_delay(50000000);

    shell_clear();

    asm volatile("sti");

    print("Welcome to Goldspace and the Gash shell!\n");
    print("Type 'help' for available commands.\n");
    print("\n");

    print("Goldspace Alpha 3.1.0, Copyright (C) 2024-2025 Goldside543\n");
    print("Goldspace comes with ABSOLUTELY NO WARRANTY.\n");
    print("This is free software, and you are welcome to redistribute it\n");
    print("under certain conditions. See the GPL-2.0 license for details.\n");
    print("\n");

   int testing = 1;

   if (testing == 1) { 
      while (1) {
           char command[256];
           size_t command_len = 0;

           if (input_len == 0) {
                asm volatile("hlt");
           }

           // Read user input
           print("> ");
           while (1) {
               char c = get_char();
               if (c == '\n' || c == '\r') {
                   command[command_len] = '\0';  // Null-terminate the command string
                   break;
               } else if (command_len < sizeof(command) - 1 && c != 0) {
                   command[command_len++] = c;
                   // Echo back the character to the screen
                   print_char(c);
               }
           }

           // Execute command
           shell_execute_command(command);
       }
   }
   else if (testing == 0) {
      while (1) {
         asm volatile("hlt");
      }
   }
}
