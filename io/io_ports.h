#ifndef IO_PORTS_H
#define IO_PORTS_H

// Write a byte to the specified I/O port
static inline void outb(unsigned short port, unsigned char value) {
    __asm__ __volatile__("outb %0, %1" : : "a"(value), "Nd"(port));
}

// Read a byte from the specified I/O port
static inline unsigned char inb(unsigned short port) {
    unsigned char value;
    __asm__ __volatile__("inb %1, %0" : "=a"(value) : "Nd"(port));
    return value;
}

#endif // IO_PORTS_H
