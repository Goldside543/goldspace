// SPDX-License-Identifier: GPL-2.0-only
/*
 * net/net-io.c
 *
 * Networking I/O functions.
 *
 * Copyright (C) 2024 Goldside543
 *
 */

#include "net-io.h"

// Output a byte (8 bits) to the given I/O port
void outb(unsigned short port, unsigned char data) {
    __asm__ __volatile__("outb %0, %1" : : "a"(data), "Nd"(port));
}

// Input a byte (8 bits) from the given I/O port
unsigned char inb(unsigned short port) {
    unsigned char data;
    __asm__ __volatile__("inb %1, %0" : "=a"(data) : "Nd"(port));
    return data;
}

// Output a word (16 bits) to the given I/O port
void outw(unsigned short port, unsigned short data) {
    __asm__ __volatile__("outw %0, %1" : : "a"(data), "Nd"(port));
}

// Input a word (16 bits) from the given I/O port
unsigned short inw(unsigned short port) {
    unsigned short data;
    __asm__ __volatile__("inw %1, %0" : "=a"(data) : "Nd"(port));
    return data;
}

// Output a long (32 bits) to the given I/O port
void outl(unsigned short port, unsigned int data) {
    __asm__ __volatile__("outl %0, %1" : : "a"(data), "Nd"(port));
}

// Input a long (32 bits) from the given I/O port
unsigned int inl(unsigned short port) {
    unsigned int data;
    __asm__ __volatile__("inl %1, %0" : "=a"(data) : "Nd"(port));
    return data;
}
