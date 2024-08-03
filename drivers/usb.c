#include "usb.h"

// USB initialization function
void usb_init(void) {
    volatile unsigned int *usb_ctrl = (volatile unsigned int *)USB_CTRL_REG;
    volatile unsigned int *usb_status = (volatile unsigned int *)USB_STATUS_REG;

    // Enable USB controller
    *usb_ctrl = USB_CTRL_ENABLE;

    // Wait for the USB controller to be ready
    while (!(*usb_status & USB_STATUS_READY)) {
        // Wait until the controller is ready
    }

    // USB controller is now initialized
}
