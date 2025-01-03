/* SPDX-License-Identifer: GPL-2.0-only */

#ifndef PCI_H
#define PCI_H

void pci_scan_bus();
uint32_t* find_rtl8139_dma_address();

#endif // PCI_H
