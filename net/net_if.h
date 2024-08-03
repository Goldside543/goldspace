#ifndef NET_IF_H
#define NET_IF_H

#include "networking.h"

// Initialize a network interface
void init_net_interface(net_interface_t *iface, const char *name, unsigned char *mac, unsigned int ip, unsigned int netmask);

// Receive a packet from the network interface
void receive_packet(net_interface_t *iface, net_packet_t *packet);

// Send a packet through the network interface
void send_packet(net_interface_t *iface, const net_packet_t *packet);

#endif // NET_IF_H
