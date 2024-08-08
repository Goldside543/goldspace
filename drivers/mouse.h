/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef MOUSE_H
#define MOUSE_H

#include <stdint.h>

// Define mouse button states
#define MOUSE_BUTTON_LEFT  0x01
#define MOUSE_BUTTON_RIGHT 0x02
#define MOUSE_BUTTON_MIDDLE 0x04

// Define a structure for mouse state
typedef struct {
    int x, y;                      // Current position of the mouse
    uint8_t buttons;               // Current button states
    int8_t x_delta, y_delta;       // Movement deltas
} MouseState;

// Function prototypes
void mouse_init(void);              // Initialize the mouse driver
void mouse_update(MouseState *state); // Update the mouse state

#endif // MOUSE_H
