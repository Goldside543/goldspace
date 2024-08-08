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

void outb(unsigned short port, unsigned char data) {
    __asm__ __volatile__("outb %0, %1" : : "a"(data), "Nd"(port));
}

unsigned char inb(unsigned short port) {
    unsigned char data;
    __asm__ __volatile__("inb %1, %0" : "=a"(data) : "Nd"(port));
    return data;
}
