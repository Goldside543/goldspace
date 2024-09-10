/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef NET_IO_H
#define NET_IO_H

// Write a byte to a hardware port
void outb(unsigned short port, unsigned char data);

// Read a byte from a hardware port
unsigned char inb(unsigned short port);

void outw(unsigned short port, unsigned short data);

unsigned short inw(unsigned short port);

void outl(unsigned short port, unsigned int data);

unsigned int inl(unsigned short port);

#endif // NET_IO_H
