// SPDX-License-Identifier: GPL-2.0-only
/*
 * drivers/keyboard.c
 *
 * USB keyboard driver.
 *
 * Copyright (C) 2024 Goldside543
 *
 */

#include "usb.h"
#include "keyboard.h"
#include "../kernel/print.h"

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
