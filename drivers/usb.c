#include <stdbool.h>
#include "usb.h"
#include "keyboard.h"  // Ensure this header file contains the declaration for keyboard_read
#include "../kernel/print.h"

// Define this variable to control which method to use
static bool use_keyboard_driver = false;

void usb_init(void) {
    volatile unsigned int *usb_ctrl = (volatile unsigned int *)USB_CTRL_REG;
    volatile unsigned int *usb_status = (volatile unsigned int *)USB_STATUS_REG;

    // Enable USB controller
    *usb_ctrl = USB_CTRL_ENABLE;
    print("Checking for USB controller...\n");

    // Wait for the USB controller to be ready
    unsigned int timeout = 3000000;  // Adjust timeout value if needed
    while (!(*usb_status & USB_STATUS_READY)) {
        if (--timeout == 0) {
            use_keyboard_driver = false;  // Timeout occurred, set to use direct I/O
            print("USB controller not found. Continuing boot without driver.\n");
            return;
        }
    }

    // USB controller is ready
    use_keyboard_driver = true;  // USB controller found, use keyboard driver
    print("USB controller driver is loaded.\n");
}
