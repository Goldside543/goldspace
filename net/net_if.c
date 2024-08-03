#include "networking.h"
#include "net-io.h"
#include "../mm/memory.h"

void init_net_interface(net_interface_t *iface, const char *name, unsigned char *mac, unsigned int ip, unsigned int netmask) {
    // Copy interface name
    for (int i = 0; i < 32 && name[i] != '\0'; i++) {
        iface->name[i] = name[i];
    }
    // Set MAC address
    for (int i = 0; i < 6; i++) {
        iface->mac_address[i] = mac[i];
    }
    iface->ip_address = ip;
    iface->netmask = netmask;
}

void receive_packet(net_interface_t *iface, net_packet_t *packet) {
    // Mock implementation
    // In reality, we would read from a hardware buffer

    unsigned char *buffer = kmalloc(1500); // Example size
    if (buffer == NULL) {
        return; // Out of memory
    }

    // Simulate receiving data
    for (int i = 0; i < 1500; i++) {
        buffer[i] = inb(0x60); // Example port
    }

    packet->data = buffer;
    packet->length = 1500; // Example length
}

void send_packet(net_interface_t *iface, const net_packet_t *packet) {
    // Mock implementation
    // In reality, we would write to a hardware buffer

    for (size_t i = 0; i < packet->length; i++) {
        outb(0x60, packet->data[i]); // Example port
    }
}
