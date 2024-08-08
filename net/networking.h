/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef NETWORKING_H
#define NETWORKING_H

#include <stddef.h>

// Define a structure for a network packet
typedef struct {
    unsigned char *data;
    size_t length;
} net_packet_t;

// Define a structure for a network interface
typedef struct {
    char name[32];
    unsigned char mac_address[6];
    unsigned int ip_address;
    unsigned int netmask;
} net_interface_t;

// Define a structure for a socket
typedef struct {
    int socket_fd;
    unsigned int local_ip;
    unsigned int remote_ip;
    unsigned short local_port;
    unsigned short remote_port;
    int state;
} net_socket_t;

#endif // NETWORKING_H
