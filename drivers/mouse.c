#include "mouse.h"
#include <stdint.h>
#include "../kernel/print.h"

#define MOUSE_DATA_PORT 0x60
#define MOUSE_COMMAND_PORT 0x64
#define TIMEOUT 3000000

// Function to read a byte from a hardware port
static uint8_t inb(uint16_t port) {
    uint8_t result;
    __asm__("inb %1, %0" : "=a"(result) : "Nd"(port));
    return result;
}

// Function to write a byte to a hardware port
static void outb(uint16_t port, uint8_t value) {
    __asm__("outb %0, %1" : : "a"(value), "Nd"(port));
}

void mouse_init(void) {
    print("Loading mouse driver...\n");

    // Wait until the mouse is ready with timeout
    int timeout = TIMEOUT;
    while (inb(MOUSE_COMMAND_PORT) & 0x02) {
        if (--timeout <= 0) {
            print("Mouse not found. Boot continuing without driver.\n");
            return;
        }
    }

    // Enable the auxiliary mouse device
    outb(MOUSE_COMMAND_PORT, 0xA8);

    // Read the current configuration byte
    outb(MOUSE_COMMAND_PORT, 0x20);
    uint8_t status = inb(MOUSE_DATA_PORT);

    // Set the configuration byte (enable interrupts)
    outb(MOUSE_COMMAND_PORT, 0x60);
    outb(MOUSE_DATA_PORT, status | 0x02);

    // Enable the mouse
    outb(MOUSE_COMMAND_PORT, 0xF4);
    outb(MOUSE_DATA_PORT, 0xF4); // Send enable command

    print("Mouse driver loaded.\n");
}

void mouse_update(MouseState *state) {
    // Wait until the mouse is ready
    while (!(inb(MOUSE_COMMAND_PORT) & 0x01)) {
        // Mouse data not ready, wait
    }

    // Read mouse data
    uint8_t mouse_data[3];
    for (int i = 0; i < 3; i++) {
        mouse_data[i] = inb(MOUSE_DATA_PORT);
    }

    // Parse mouse data
    state->buttons = mouse_data[0] & 0x07;
    state->x_delta = mouse_data[1];
    state->y_delta = -mouse_data[2]; // Y axis is inverted

    // Update position
    state->x += state->x_delta;
    state->y += state->y_delta;
}
