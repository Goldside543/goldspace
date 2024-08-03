#ifndef NET-IO_H
#define NET-IO_H

// Write a byte to a hardware port
void outb(unsigned short port, unsigned char data);

// Read a byte from a hardware port
unsigned char inb(unsigned short port);

#endif // NET-IO_H
