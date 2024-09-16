/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef ARP_H
#define ARP_H

uint16_t htons(uint16_t hostshort);
uint32_t htonl(uint32_t hostlong);

void send_ethernet_frame(net_interface_t *iface, void *data, size_t len, const uint8_t *dest_mac);
void send_arp_request(net_interface_t *iface, uint32_t target_ip);

uint16_t ntohs(uint16_t netshort);

void add_to_arp_cache(uint32_t ip_address, uint8_t *mac_address);
void handle_arp_packet(net_interface_t *iface, struct arp_packet *packet);
void add_to_arp_cache(uint32_t ip_address, uint8_t *mac_address);
uint8_t *lookup_arp_cache(uint32_t ip_address);

#endif // ARP_H
