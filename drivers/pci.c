// SPDX-License-Identifier: GPL-2.0-only
/*
 * drivers/pci.c
 * 
 * Functions for scanning the PCI bus.
 *
 * Copyright (C) 2024 Goldside543
 *
 */

#include "../kernel/print.h"
#include "../kernel/io.h"
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#define PCI_CONFIG_ADDRESS 0xCF8
#define PCI_CONFIG_DATA    0xCFC

// Helper function to convert an integer to a string
void itoa(uint32_t num, char* str, int base) {
    int i = 0;
    int isNegative = 0;

    // Handle 0 explicitly, otherwise empty string is printed
    if (num == 0) {
        str[i++] = '0';
        str[i] = '\0';
        return;
    }

    // Handle negative numbers only if base is 10
    if (num < 0 && base == 10) {
        isNegative = 1;
        num = -num;
    }

    // Process individual digits
    while (num != 0) {
        int rem = num % base;
        str[i++] = (rem > 9) ? (rem - 10) + 'a' : rem + '0';
        num = num / base;
    }

    // Append negative sign for negative numbers
    if (isNegative) {
        str[i++] = '-';
    }

    str[i] = '\0'; // Null-terminate string

    // Reverse the string
    int start = 0;
    int end = i - 1;
    while (start < end) {
        char temp = str[start];
        str[start] = str[end];
        str[end] = temp;
        start++;
        end--;
    }
}

// Helper function to create a PCI configuration address
uint32_t pci_config_address(uint8_t bus, uint8_t device, uint8_t function, uint8_t offset) {
    return (1U << 31) |        // Enable bit
           ((uint32_t)bus << 16) |
           ((uint32_t)device << 11) |
           ((uint32_t)function << 8) |
           (offset & 0xFC);    // Align to 4-byte boundary
}

// Helper function to read 32 bits from the PCI configuration space
uint32_t pci_read_config(uint8_t bus, uint8_t device, uint8_t function, uint8_t offset) {
    uint32_t address = pci_config_address(bus, device, function, offset);
    outl(PCI_CONFIG_ADDRESS, address);  // Write to CONFIG_ADDRESS
    return inl(PCI_CONFIG_DATA);       // Read from CONFIG_DATA
}

void pci_scan_bus() {
    for (uint16_t bus = 0; bus < 256; ++bus) {
        for (uint8_t device = 0; device < 32; ++device) {
            // First check function 0 for the device presence
            uint32_t vendor_device = pci_read_config(bus, device, 0, 0x00);

            // If function 0 has no device (vendor_id == 0xFFFF), skip this device
            if ((vendor_device & 0xFFFF) == 0xFFFF) {
                continue; // No device present on this device slot, skip to next device
            }

            // Now check for all functions (0-7) for this device
            bool found_device = false;  // Flag to check if at least one function is found
            for (uint8_t function = 0; function < 8; ++function) {
                // Read the vendor/device ID for the current function
                vendor_device = pci_read_config(bus, device, function, 0x00);

                // If vendor_id is 0xFFFF, it means no device in this function
                if ((vendor_device & 0xFFFF) == 0xFFFF) {
                    continue; // This function does not exist, so skip to next function
                }

                found_device = true;  // Mark that a function was found for this device

                // Device is present, proceed to fetch other details
                uint16_t vendor_id = vendor_device & 0xFFFF;
                uint16_t device_id = (vendor_device >> 16) & 0xFFFF;

                uint32_t class_subclass = pci_read_config(bus, device, function, 0x08);
                uint8_t class_code = (class_subclass >> 24) & 0xFF;
                uint8_t subclass_code = (class_subclass >> 16) & 0xFF;

                // Allocate string buffers to hold the values as strings
                char buffer[32];  // Sufficient for 32-bit integer

                // Print the values using the itoa function
                print("\nBus: ");
                itoa(bus, buffer, 10); // Convert bus to string
                print(buffer);

                print(" Device: ");
                itoa(device, buffer, 10); // Convert device to string
                print(buffer);

                print(" Vendor ID: ");
                print("0x");
                itoa(vendor_id, buffer, 16); // Convert vendor_id to hex string
                print(buffer);

                print(" Device ID: ");
                print("0x");
                itoa(device_id, buffer, 16); // Convert device_id to hex string
                print(buffer);

                print(" Class: ");
                print("0x");
                itoa(class_code, buffer, 16); // Convert class_code to hex string
                print(buffer);

                print(" Subclass: ");
                print("0x");
                itoa(subclass_code, buffer, 16); // Convert subclass_code to hex string
                print(buffer);

                print("\n");
            }

            // If no valid functions were found for this device, skip to the next device
            if (!found_device) {
                continue;
            }
        }
    }
}

uint32_t* find_rtl8139_dma_address() {
    for (uint16_t bus = 0; bus < 256; ++bus) {
        for (uint8_t device = 0; device < 32; ++device) {
            // Check function 0 for the device presence
            uint32_t vendor_device = pci_read_config(bus, device, 0, 0x00);

            // If vendor_id is 0xFFFF, no device present in this slot
            if ((vendor_device & 0xFFFF) == 0xFFFF) {
                continue;
            }

            // Now check all functions for this device
            for (uint8_t function = 0; function < 8; ++function) {
                vendor_device = pci_read_config(bus, device, function, 0x00);

                if ((vendor_device & 0xFFFF) == 0xFFFF) {
                    continue;  // Skip if no device present
                }

                // Check if this is an RTL8139 device (vendor 0x10EC, device 0x8139)
                uint16_t vendor_id = vendor_device & 0xFFFF;
                uint16_t device_id = (vendor_device >> 16) & 0xFFFF;

                if (vendor_id == 0x10EC && device_id == 0x8139) {
                    // Found RTL8139, let's read the Base Address Register (BAR)
                    uint32_t bar0 = pci_read_config(bus, device, function, 0x10);  // BAR0 (Memory Base Address)
                    
                    // Check if BAR0 is valid (non-zero)
                    if (bar0 != 0) {
                        // Assuming BAR0 contains the DMA address (memory-mapped)
                        print("Found RTL8139! DMA Base Address: ");
                        char buffer[32];
                        itoa(bar0, buffer, 16);  // Convert the DMA address to hex string
                        print(buffer);
                        print("\n");

                        return (uint32_t*)bar0;  // Return the DMA base address
                    }
                }
            }
        }
    }

    print("RTL8139 not found.\n");
    return NULL;  // Return NULL if RTL8139 is not found
}
