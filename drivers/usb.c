void usb_init(void) {
    volatile unsigned int *usb_ctrl = (volatile unsigned int *)USB_CTRL_REG;
    volatile unsigned int *usb_status = (volatile unsigned int *)USB_STATUS_REG;

    // Enable USB controller
    *usb_ctrl = USB_CTRL_ENABLE;
    print("USB controller enabled.\n");

    // Wait for the USB controller to be ready
    unsigned int timeout = 1000000;
    while (!(*usb_status & USB_STATUS_READY)) {
        if (--timeout == 0) {
            print("USB controller initialization timed out.\n");
            return;
        }
    }
    print("USB controller is ready.\n");
