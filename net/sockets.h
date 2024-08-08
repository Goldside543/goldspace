/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef SOCKETS_H
#define SOCKETS_H

#include "networking.h"

// Create a new socket
net_socket_t* create_socket();

// Bind a socket to a local IP address and port
int bind_socket(net_socket_t *sock, unsigned int ip, unsigned short port);

// Connect a socket to a remote IP address and port
int connect_socket(net_socket_t *sock, unsigned int remote_ip, unsigned short remote_port);

// Send data through a socket
int send_data(net_socket_t *sock, const unsigned char *data, size_t length);

// Receive data from a socket
int receive_data(net_socket_t *sock, unsigned char *buffer, size_t buffer_size);

#endif // SOCKETS_H
