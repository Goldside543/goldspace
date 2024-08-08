/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef PRINT_H
#define PRINT_H

#include <stdint.h>

#define VGA_WIDTH 80
#define VGA_HEIGHT 25

// Function to print a string to the VGA text mode display
void print(const char *str);

#endif // PRINT_H
