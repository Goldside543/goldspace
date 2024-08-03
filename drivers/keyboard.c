#include "usb.h"
#include "keyboard.h"

// Read a character from the keyboard
char read_keyboard(void) {
    volatile unsigned int *keyboard_status = (volatile unsigned int *)USB_KEYBOARD_STATUS;
    volatile unsigned int *keyboard_data = (volatile unsigned int *)USB_KEYBOARD_DATA;

    // Wait for data to be ready
    while (!(*keyboard_status & USB_KEYBOARD_STATUS_READY)) {
        // Wait until data is ready
    }

    // Read the keyboard data
    return (char)(*keyboard_data);
}
