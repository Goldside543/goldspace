/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef IO_H
#define IO_H

#include <stdint.h>

// Function to read a byte from an I/O port
static inline uint8_t inb(uint16_t port) {
    uint8_t result;
    __asm__ __volatile__ ("inb %1, %0" : "=a"(result) : "Nd"(port));
    return result;
}

// Function to write a byte to an I/O port
static inline void outb(uint16_t port, uint8_t data) {
    __asm__ __volatile__ ("outb %0, %1" : : "a"(data), "Nd"(port));
}

static inline void outw(unsigned short port, unsigned short data) {
    __asm__ __volatile__("outw %0, %1" : : "a"(data), "Nd"(port));
}
#endif // IO_H
