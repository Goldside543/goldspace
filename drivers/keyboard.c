#include "usb.h"
#include "keyboard.h"
#include "../kernel/print.h"

#define USB_KEYBOARD_RESET_CMD 0xFF 
#define USB_KEYBOARD_ENABLE_CMD 0xF4 

void init_keyboard(void) {
    volatile unsigned int *usb_keyboard_ctrl = (volatile unsigned int *)USB_KEYBOARD_CTRL;
    volatile unsigned int *usb_keyboard_data = (volatile unsigned int *)USB_KEYBOARD_DATA;
    volatile unsigned int *usb_keyboard_status = (volatile unsigned int *)USB_KEYBOARD_STATUS;

    print("Initializing USB keyboard...\n");

    // Send reset command to keyboard
    *usb_keyboard_data = USB_KEYBOARD_RESET_CMD;
    print("Sent reset command to keyboard.\n");

    // Wait for the reset to complete
    unsigned int timeout = 1000000;
    while (!(*usb_keyboard_status & USB_KEYBOARD_STATUS_READY)) {
        if (--timeout == 0) {
            print("Timeout waiting for keyboard reset.\n");
            return;
        }
    }
    print("Keyboard reset complete.\n");

    // Send enable command to keyboard
    *usb_keyboard_data = USB_KEYBOARD_ENABLE_CMD;
    print("Sent enable command to keyboard.\n");

    // Wait for the initialization to complete
    timeout = 1000000;
    while (!(*usb_keyboard_status & USB_KEYBOARD_STATUS_READY)) {
        if (--timeout == 0) {
            print("Timeout waiting for keyboard initialization.\n");
            return;
        }
    }

    print("Keyboard initialization complete.\n");
}