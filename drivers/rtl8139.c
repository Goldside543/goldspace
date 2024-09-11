// SPDX-License-Identifier: GPL-2.0-only
/*
 * drivers/rtl8139.c
 *
 * RTL8139 driver.
 *
 * Copyright (C) 2024 Goldside543
 *
 */

#include "../net/networking.h"
#include "../net/net-io.h"
#include "../mm/memory.h"

// RTL8139-specific defines
#define RTL8139_CMD 0x37
#define RTL8139_IMR 0x3C
#define RTL8139_ISR 0x3E
#define RTL8139_RX_BUFFER 0x30
#define RTL8139_TX_BUFFER 0x20
#define RTL8139_RX_CONFIG 0x44

// I/O base address for the RTL8139 card (to be set after PCI scan)
static unsigned int io_base;

// Function to initialize the RTL8139 card
void rtl8139_init() {
    // Reset the card
    outb(io_base + RTL8139_CMD, 0x10);
    while (inb(io_base + RTL8139_CMD) & 0x10) {
        // Wait for reset to complete
    }

    // Allocate and set RX buffer
    unsigned char *rx_buffer = kmalloc(32768);  // 32 KB
    outl(io_base + RTL8139_RX_BUFFER, (uint32_t)rx_buffer);

    // Configure RX buffer to accept all packets
    outl(io_base + RTL8139_RX_CONFIG, 0x00000F00);  // RX config: accept all

    // Enable RX and TX interrupts
    outw(io_base + RTL8139_IMR, 0x0005);
}

// Initialize network interface
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

    // Initialize RTL8139 hardware
    rtl8139_init();
}

// Receive packet from the RTL8139 card
void receive_packet(net_interface_t *iface, net_packet_t *packet) {
    // Read from RX buffer if there's a packet
    uint16_t status = inw(io_base + RTL8139_ISR);
    if (status & 0x01) {  // Packet received
        // Read packet length and data
        unsigned int length = inw(io_base + RTL8139_RX_BUFFER);
        unsigned char *buffer = kmalloc(length);  // Allocate memory for packet
        if (buffer == NULL) {
            return;  // Out of memory
        }

        // Read the actual packet data
        for (unsigned int i = 0; i < length; i++) {
            buffer[i] = inb(io_base + RTL8139_RX_BUFFER + 4 + i);  // 4-byte offset
        }

        // Fill packet structure
        packet->data = buffer;
        packet->length = length;

        // Acknowledge the interrupt
        outw(io_base + RTL8139_ISR, 0x01);
    }
}

// Send packet using the RTL8139 card
void send_packet(net_interface_t *iface, const net_packet_t *packet) {
    // Transmit the packet
    for (int tx_desc = 0; tx_desc < 4; tx_desc++) {  // RTL8139 has 4 TX descriptors
        // Write packet data to the TX buffer
        for (unsigned int i = 0; i < packet->length; i++) {
            outb(io_base + RTL8139_TX_BUFFER + (tx_desc * 4), packet->data[i]);
        }

        // Trigger transmission by writing the packet length
        outl(io_base + RTL8139_TX_BUFFER + (tx_desc * 4), packet->length);
    }
}
