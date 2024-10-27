// SPDX-License-Identifier: GPL-2.0-only
/*
 * drivers/disk.c
 * 
 * ATA PIO driver. Not really functional.
 *
 * Copyright (C) 2024 Goldside543
 *
 */

#include "../kernel/io.h"
#include "disk.h"

void ata_pio_select_drive(uint8_t drive) {
    outb(ATA_REG_DRIVE_SELECT, 0xA0 | (drive << 4));
}

void ata_pio_wait_ready() {
    uint8_t status;
    // Read 15 times and pay attention to the last
    for (int i = 0; i < 15; i++) {
        status = inb(ATA_REG_STATUS);
    }
    
    // Ensure the drive is not busy and is ready
    while (status & ATA_STATUS_BUSY) {
        status = inb(ATA_REG_STATUS);
    }
}

void ata_pio_init() {
    outb(0x3F6, 0);    

    // Select the primary master drive
    ata_pio_select_drive(0);

    // Send IDENTIFY command to the drive
    ata_pio_wait_ready();
    outb(ATA_REG_COMMAND, ATA_COMMAND_IDENTIFY);

    // Wait for the drive to be ready
    ata_pio_wait_ready();

}

int ata_pio_read(uint32_t lba, void *buffer, size_t size) {
    uint16_t *buf = (uint16_t *)buffer;  // Use 16-bit buffer for 16-bit reads
    ata_pio_wait_ready();
    ata_pio_select_drive(0);  // Select primary master drive

    // Setup LBA address
    outb(ATA_REG_SECTOR_COUNT, 1);  // Read one sector (512 bytes)
    outb(ATA_REG_LBA_LOW, (lba & 0xFF));
    outb(ATA_REG_LBA_MID, (lba >> 8) & 0xFF);
    outb(ATA_REG_LBA_HIGH, (lba >> 16) & 0xFF);
    outb(ATA_REG_COMMAND, ATA_COMMAND_READ_SECTORS);

    // Wait for the drive to signal it's ready for data transfer
    if (!(inb(ATA_REG_STATUS) & ATA_STATUS_DRQ)) {
        return -1; // Error: Disk not ready for read
    }

    // Read the full sector (512 bytes = 256 words of 16 bits)
    for (size_t i = 0; i < 256; i++) {  // 256 words per sector
        if (inb(ATA_REG_STATUS) & ATA_STATUS_DRQ) {
            *buf++ = inw(ATA_REG_DATA);  // Read 16 bits (2 bytes) at a time
        } else {
            return -2; // Error: Disk read failed during data transfer
        }
    }

    return 0; // Success
}

int ata_pio_write(uint32_t lba, const void *buffer, size_t size) {
    const uint16_t *buf = (const uint16_t *)buffer;
    ata_pio_wait_ready();
    ata_pio_select_drive(0); // Select primary master drive

    // Setup LBA address
    outb(ATA_REG_SECTOR_COUNT, 1); // Writing one sector at a time (512 bytes)
    outb(ATA_REG_LBA_LOW, (lba & 0xFF));
    outb(ATA_REG_LBA_MID, (lba >> 8) & 0xFF);
    outb(ATA_REG_LBA_HIGH, (lba >> 16) & 0xFF);
    outb(ATA_REG_COMMAND, ATA_COMMAND_WRITE_SECTORS);

    // Wait until the drive is ready for data transfer
    if (!(inb(ATA_REG_STATUS) & ATA_STATUS_DRQ)) {
        return -3; // Error: Disk not ready for write
    }

    // Write the data sector (512 bytes)
    for (size_t i = 0; i < 256; i++) { // 256 words (512 bytes per sector)
        if (inb(ATA_REG_STATUS) & ATA_STATUS_DRQ) {
            outw(ATA_REG_DATA, *buf++); // Write 16 bits (2 bytes) at a time
        } else {
            return -4; // Error: Disk write failed during data transfer
        }
    }

    // Send the flush command to ensure the data is written
    outb(ATA_REG_COMMAND, ATA_COMMAND_CACHE_FLUSH);
    ata_pio_wait_ready();

    // Check if flush command succeeded
    if (!(inb(ATA_REG_STATUS) & ATA_STATUS_DRQ)) {
        return -5; // Error: Disk write flush failed
    }

    return 0; // Success
}
