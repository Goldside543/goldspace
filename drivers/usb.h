#ifndef USB_H
#define USB_H

#define USB_CONTROLLER_BASE 0xFCBA0000
#define USB_CTRL_REG    (USB_CONTROLLER_BASE + 0x00)
#define USB_STATUS_REG  (USB_CONTROLLER_BASE + 0x04)

// USB controller settings
#define USB_CTRL_ENABLE 0x01
#define USB_STATUS_READY 0x01

// Function declarations
void usb_init(void);

#endif // USB_H
