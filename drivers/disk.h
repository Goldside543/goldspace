/* SPDX-License-Identifer: GPL-2.0-only */

#ifndef DISK_H
#define DISK_H

#include <stdint.h>
#include <stddef.h>

// ATA PIO Registers (Primary and Secondary)
#define ATA_REG_DATA          0x1F0
#define ATA_REG_ERROR         0x1F1
#define ATA_REG_FEATURE       0x1F1
#define ATA_REG_SECTOR_COUNT  0x1F2
#define ATA_REG_LBA_LOW       0x1F3
#define ATA_REG_LBA_MID       0x1F4
#define ATA_REG_LBA_HIGH      0x1F5
#define ATA_REG_DRIVE_SELECT  0x1F6
#define ATA_REG_COMMAND       0x1F7
#define ATA_REG_STATUS        0x1F7

// ATA Commands
#define ATA_COMMAND_IDENTIFY   0xEC
#define ATA_COMMAND_READ_SECTORS    0x20
#define ATA_COMMAND_WRITE_SECTORS   0x30
#define ATA_COMMAND_CACHE_FLUSH    0xE7

// ATA Status Register Bits
#define ATA_STATUS_BUSY       0x80
#define ATA_STATUS_DRQ        0x08
#define ATA_STATUS_READY      0x40
#define ATA_STATUS_ERROR      0x01

// Function prototypes
void ata_pio_select_drive(uint8_t drive);
void ata_pio_wait_ready(void);
int ata_pio_read(uint32_t lba, void *buffer, size_t size);
int ata_pio_write(uint32_t lba, const void *buffer, size_t size);
void ata_pio_init();

#endif // DISK_H
