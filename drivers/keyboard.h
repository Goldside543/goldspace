#ifndef KEYBOARD_H
#define KEYBOARD_H

// Define base address for USB keyboard controller
#define USB_KEYBOARD_CONTROLLER_BASE 0xFCBB0000  // Update this address based on your system

// Define offsets for the keyboard control registers
#define USB_KEYBOARD_CTRL   (USB_KEYBOARD_CONTROLLER_BASE + 0x00) // Control register
#define USB_KEYBOARD_DATA   (USB_KEYBOARD_CONTROLLER_BASE + 0x04) // Data register
#define USB_KEYBOARD_STATUS (USB_KEYBOARD_CONTROLLER_BASE + 0x08) // Status register

// Define keyboard commands (update with actual values as needed)
#define USB_KEYBOARD_RESET_CMD 0xFF  // Command to reset the keyboard
#define USB_KEYBOARD_ENABLE_CMD 0xF4 // Command to enable the keyboard

// Define status flags (update with actual values as needed)
#define USB_KEYBOARD_STATUS_READY 0x01  // Status flag indicating the keyboard is ready

void init_keyboard(void);
char read_keyboard(void);

#endif // KEYBOARD_H
