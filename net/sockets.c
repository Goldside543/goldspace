#include "networking.h"
#include "../mm/memory.h"

net_socket_t* create_socket() {
    net_socket_t *sock = (net_socket_t*)kmalloc(sizeof(net_socket_t));
    if (sock != NULL) {
        sock->socket_fd = 0; // Placeholder for file descriptor
        sock->state = 0; // Placeholder for socket state
    }
    return sock;
}

int bind_socket(net_socket_t *sock, unsigned int ip, unsigned short port) {
    sock->local_ip = ip;
    sock->local_port = port;
    return 0; // Success
}

int connect_socket(net_socket_t *sock, unsigned int remote_ip, unsigned short remote_port) {
    sock->remote_ip = remote_ip;
    sock->remote_port = remote_port;
    return 0; // Success
}

int send_data(net_socket_t *sock, const unsigned char *data, size_t length) {
    // Create a packet
    net_packet_t packet;
    packet.data = (unsigned char*)data;
    packet.length = length;

    // Send the packet
    // For simplicity, assuming interface index 0
    net_interface_t iface;
    send_packet(&iface, &packet);
    
    return 0; // Success
}

int receive_data(net_socket_t *sock, unsigned char *buffer, size_t buffer_size) {
    // Create a packet
    net_packet_t packet;
    receive_packet(NULL, &packet); // Placeholder for interface
    
    if (packet.length > buffer_size) {
        return -1; // Buffer too small
    }

    // Copy data
    for (size_t i = 0; i < packet.length; i++) {
        buffer[i] = packet.data[i];
    }

    return packet.length; // Return number of bytes received
}
