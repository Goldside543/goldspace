// SPDX-License-Identifier: GPL-2.0-only
/*
 * net/arp.c
 *
 * Implementation of ARP.
 *
 * Copyright (C) 2024 Goldside543
 *
 */

#include <stdint.h>
#include "../mm/memory.h"
#include "net-io.h"
#include "../drivers/rtl8139.h"
#include "../kernel/print.h"

#define ARP_CACHE_SIZE 16

// Convert 16-bit value from host to network byte order
uint16_t htons(uint16_t hostshort) {
    return (hostshort >> 8) | (hostshort << 8);
}

// Convert 32-bit value from host to network byte order
uint32_t htonl(uint32_t hostlong) {
    return ((hostlong & 0x000000FF) << 24) |
           ((hostlong & 0x0000FF00) << 8)  |
           ((hostlong & 0x00FF0000) >> 8)  |
           ((hostlong & 0xFF000000) >> 24);
}

// Define ARP packet structure
struct arp_packet {
    uint16_t hw_type;
    uint16_t protocol_type;
    uint8_t hw_addr_len;
    uint8_t proto_addr_len;
    uint16_t operation;
    uint8_t sender_mac[6];
    uint32_t sender_ip;
    uint8_t target_mac[6];
    uint32_t target_ip;
} __attribute__((packed));

struct eth_header {
    uint8_t dest_mac[6];
    uint8_t src_mac[6];
    uint16_t ethertype;  // E.g., 0x0800 for IPv4
} __attribute__((packed));

void send_ethernet_frame(net_interface_t *iface, void *data, size_t len, const uint8_t *dest_mac) {
    // Ensure the length is within the acceptable range (for example, 1500 bytes for MTU)
    if (len > 1500) {
        print("Packet too large\n");
        return;
    }

    // Prepare the Ethernet header
    struct eth_header header;
    kmemcpy(header.dest_mac, dest_mac, 6);
    kmemcpy(header.src_mac, iface->mac_address, 6);
    header.ethertype = htons(0x0800);  // Set Ethertype to IPv4

    // Allocate buffer for the full frame (header + data)
    unsigned char *frame = kmalloc(len + sizeof(header));
    if (!frame) {
        print("Failed to allocate frame buffer\n");
        return;
    }
    
    // Copy header and data to the frame buffer
    kmemcpy(frame, &header, sizeof(header));
    kmemcpy(frame + sizeof(header), data, len);

    // Create a net_packet_t structure to pass to send_packet
    net_packet_t packet;
    packet.data = frame;
    packet.length = len + sizeof(header);

    // Send the frame using send_packet
    send_packet(iface, &packet);

    kfree(frame);  // Free the allocated frame buffer
}

// Function to send an ARP request
void send_arp_request(net_interface_t *iface, uint32_t target_ip) {
    struct arp_packet packet;

    // Set hardware type (Ethernet)
    packet.hw_type = htons(0x0001);

    // Set protocol type (IPv4)
    packet.protocol_type = htons(0x0800);

    // Set hardware and protocol address lengths
    packet.hw_addr_len = 6;   // MAC address length
    packet.proto_addr_len = 4;  // IP address length

    // Set operation (1 for ARP request)
    packet.operation = htons(0x0001);

    // Set sender's MAC and IP addresses
    kmemcpy(packet.sender_mac, iface->mac_address, 6);
    packet.sender_ip = htonl(iface->ip_address);

    // Set target MAC address to zero (unknown)
    kmemset(packet.target_mac, 0, 6);

    // Set target IP address (the IP we want to resolve)
    packet.target_ip = htonl(target_ip);

    // Send ARP packet in an Ethernet frame
    send_ethernet_frame(iface, packet, sizeof(packet), broadcast_mac);
}

// Convert 16-bit value from network to host byte order
uint16_t ntohs(uint16_t netshort) {
    return (netshort >> 8) | (netshort << 8);
}

// Function to handle incoming ARP packet
void handle_arp_packet(net_interface_t *iface, struct arp_packet *packet) {
    // Ensure the operation is an ARP reply (2)
    if (ntohs(packet->operation) != 0x0002) {
        return;
    }

    // Add the sender's MAC and IP to the ARP cache
    add_to_arp_cache(packet->sender_ip, packet->sender_mac);
}

struct arp_entry {
    uint32_t ip_address;
    uint8_t mac_address[6];
};

struct arp_entry arp_cache[ARP_CACHE_SIZE];

// Function to add entry to the ARP cache
void add_to_arp_cache(uint32_t ip_address, uint8_t *mac_address) {
    for (int i = 0; i < ARP_CACHE_SIZE; i++) {
        if (arp_cache[i].ip_address == 0) {
            arp_cache[i].ip_address = ip_address;
            kmemcpy(arp_cache[i].mac_address, mac_address, 6);
            return;
        }
    }
}

// Function to lookup MAC address in the ARP cache
uint8_t *lookup_arp_cache(uint32_t ip_address) {
    for (int i = 0; i < ARP_CACHE_SIZE; i++) {
        if (arp_cache[i].ip_address == ip_address) {
            return arp_cache[i].mac_address;
        }
    }
    return NULL;  // Not found
}
