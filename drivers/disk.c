#include "../kernel/io.h"
#include <stddef.h>
#include <stdint.h>

#define ATA_REG_DATA    0x1F0
#define ATA_REG_ERROR   0x1F1
#define ATA_REG_FEATURE 0x1F1
#define ATA_REG_SECTOR_COUNT 0x1F2
#define ATA_REG_LBA_LOW 0x1F3
#define ATA_REG_LBA_MID 0x1F4
#define ATA_REG_LBA_HIGH 0x1F5
#define ATA_REG_DRIVE_SELECT 0x1F6
#define ATA_REG_COMMAND 0x1F7
#define ATA_REG_STATUS 0x1F7

#define ATA_COMMAND_READ_SECTORS 0x20
#define ATA_COMMAND_WRITE_SECTORS 0x30
#define ATA_STATUS_BUSY 0x80
#define ATA_STATUS_DRQ 0x08
#define ATA_STATUS_READY 0x40
#define ATA_STATUS_ERROR 0x01

void ata_pio_select_drive(uint8_t drive) {
    outb(ATA_REG_DRIVE_SELECT, 0xA0 | (drive << 4));
}

void ata_pio_wait_ready() {
    while (inb(ATA_REG_STATUS) & ATA_STATUS_BUSY);
}

void ata_pio_read(uint32_t lba, void *buffer, size_t size) {
    uint8_t *buf = (uint8_t *)buffer;
    ata_pio_wait_ready();
    ata_pio_select_drive(0);

    // Setup LBA address
    outb(ATA_REG_SECTOR_COUNT, 1);
    outb(ATA_REG_LBA_LOW, (lba & 0xFF));
    outb(ATA_REG_LBA_MID, (lba >> 8) & 0xFF);
    outb(ATA_REG_LBA_HIGH, (lba >> 16) & 0xFF);
    outb(ATA_REG_COMMAND, ATA_COMMAND_READ_SECTORS);

    ata_pio_wait_ready();
    while (size--) {
        if (inb(ATA_REG_STATUS) & ATA_STATUS_DRQ) {
            *buf++ = inb(ATA_REG_DATA);
        }
    }
}

void ata_pio_write(uint32_t lba, const void *buffer, size_t size) {
    const uint8_t *buf = (const uint8_t *)buffer;
    ata_pio_wait_ready();
    ata_pio_select_drive(0); // Select primary master drive

    // Setup LBA address
    outb(ATA_REG_SECTOR_COUNT, 1);
    outb(ATA_REG_LBA_LOW, (lba & 0xFF));
    outb(ATA_REG_LBA_MID, (lba >> 8) & 0xFF);
    outb(ATA_REG_LBA_HIGH, (lba >> 16) & 0xFF);
    outb(ATA_REG_COMMAND, ATA_COMMAND_WRITE_SECTORS);

    ata_pio_wait_ready();
    while (size--) {
        if (inb(ATA_REG_STATUS) & ATA_STATUS_DRQ) {
            outb(ATA_REG_DATA, *buf++);
        }
    }
}
