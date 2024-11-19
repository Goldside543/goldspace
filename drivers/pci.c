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

#define PCI_CONFIG_ADDRESS 0xCF8
#define PCI_CONFIG_DATA    0xCFC

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

// Main PCI scanning function
void pci_scan_bus() {
    for (uint8_t bus = 0; bus < 256; ++bus) {
        for (uint8_t device = 0; device < 32; ++device) {
            for (uint8_t function = 0; function < 8; ++function) {
                uint32_t vendor_device = pci_read_config(bus, device, function, 0x00);

                // Check if a device is present
                if ((vendor_device & 0xFFFF) != 0xFFFF) {  // Vendor ID != 0xFFFF
                    uint16_t vendor_id = vendor_device & 0xFFFF;
                    uint16_t device_id = (vendor_device >> 16) & 0xFFFF;

                    uint32_t class_subclass = pci_read_config(bus, device, function, 0x08);
                    uint8_t class_code = (class_subclass >> 24) & 0xFF;
                    uint8_t subclass_code = (class_subclass >> 16) & 0xFF;

                    print("PCI Device Found:\n");
                    print("  Bus: %d, Device: %d, Function: %d\n", bus, device, function);
                    print("  Vendor ID: 0x%X, Device ID: 0x%X\n", vendor_id, device_id);
                    print("  Class: 0x%X, Subclass: 0x%X\n", class_code, subclass_code);
                    print("\n");
                }
            }
        }
    }
}
