// SPDX-License-Identifier: GPL-2.0-only
/*
 * fs/fat32/fat32.c
 *
 * FAT32 implementation.
 *
 * Copyright (C) 2024 Goldside543
 *
 */

#include "fat32.h"
#include "../../kernel/string.h"
#include "../../drivers/disk.h"
#include "../../mm/memory.h"

#define SECTOR_SIZE 512
#define FAT32_MAX_FILES 128

// FAT32 constants
#define FAT32_SIGNATURE 0xAA55
#define FAT32_BOOTSECTOR_SIZE 512
#define FAT32_ENTRY_SIZE 32 // Size of a directory entry in bytes

// FAT32 Boot Sector structure
typedef struct __attribute__((packed)) {
    uint8_t jump[3];
    char oem[8];
    uint16_t bytes_per_sector;
    uint8_t sectors_per_cluster;
    uint16_t reserved_sectors;
    uint8_t num_fats;
    uint16_t max_root_dir_entries;
    uint16_t total_sectors_16;
    uint8_t media_descriptor;
    uint16_t fat_size_16;
    uint16_t sectors_per_track;
    uint16_t number_of_heads;
    uint32_t hidden_sectors;
    uint32_t total_sectors_32;
    uint32_t fat_size_32;
    uint16_t extended_flags;
    uint16_t filesystem_version;
    uint32_t root_cluster;
    uint16_t filesystem_info;
    uint16_t backup_boot_sector;
    uint8_t reserved[12];
    uint8_t drive_number;
    uint8_t reserved1;
    uint8_t boot_signature;
    uint32_t volume_id;
    char volume_label[11];
    char filesystem_type[8];
    uint8_t reserved2[453];
    uint16_t boot_sector_signature; // Should be 0xAA55
} __attribute__((packed)) FAT32_BootSector;

typedef struct {
    uint32_t first_cluster; // Starting cluster of the file
    uint32_t size;          // Size of the file in bytes
} FileEntry;

// Global variables
static FAT32_BootSector boot_sector;
static uint32_t fat_start;
static uint32_t data_start;

// Mount the FAT32 file system
bool fat32_mount() {
    // Read the boot sector
    ata_pio_read(0, &boot_sector, FAT32_BOOTSECTOR_SIZE);
    if (boot_sector.boot_sector_signature != FAT32_SIGNATURE) {
        return false; // Not a FAT32 filesystem
    }

    // Initialize variables
    fat_start = boot_sector.reserved_sectors * boot_sector.bytes_per_sector;
    data_start = fat_start + (boot_sector.num_fats * boot_sector.fat_size_32 * boot_sector.bytes_per_sector);
    return true;
}

// Get the cluster size
static uint32_t get_cluster_size() {
    return boot_sector.sectors_per_cluster * boot_sector.bytes_per_sector;
}

// Get a FAT entry for a given cluster
static uint32_t get_fat_entry(uint32_t cluster) {
    uint32_t fat_offset = cluster * 4; // 4 bytes per entry
    uint32_t fat_sector = fat_start + (fat_offset / SECTOR_SIZE);
    uint32_t fat_offset_within_sector = fat_offset % SECTOR_SIZE;
    uint32_t fat_entry;

    ata_pio_read(fat_sector, &fat_entry, sizeof(fat_entry));
    return fat_entry & 0x0FFFFFFF; // Mask to get only the valid bits
}

// Set a FAT entry for a given cluster
static void set_fat_entry(uint32_t cluster, uint32_t value) {
    uint32_t fat_offset = cluster * 4; // 4 bytes per entry
    uint32_t fat_sector = fat_start + (fat_offset / SECTOR_SIZE);
    uint32_t fat_offset_within_sector = fat_offset % SECTOR_SIZE;

    uint32_t fat_entry;
    ata_pio_read(fat_sector, &fat_entry, sizeof(fat_entry));
    fat_entry = value; // Set new value

    ata_pio_write(fat_sector, &fat_entry, sizeof(fat_entry));
}

// Find a file in the root directory or subdirectories
static uint32_t find_file(const char *filename) {
    uint32_t cluster = boot_sector.root_cluster;
    uint32_t entry_offset = 0;

    while (true) {
        uint32_t cluster_size = get_cluster_size();
        uint8_t buffer[cluster_size];
        read_clusters(cluster, buffer, cluster_size);

        for (size_t i = 0; i < cluster_size; i += FAT32_ENTRY_SIZE) {
            // Read the directory entry
            if (buffer[i] == 0x00) { // End of directory entries
                return 0; // File not found
            }
            if (buffer[i] == 0xE5 || buffer[i] == 0x00) continue; // Deleted or empty entry

            char entry_name[12];
            kmemcpy(entry_name, buffer + i, 11);
            entry_name[11] = '\0'; // Null-terminate the string

            if (my_strcmp(entry_name, filename) == 0) {
                return *((uint32_t *)(buffer + i + 26)); // Starting cluster from entry
            }
        }

        cluster = get_fat_entry(cluster); // Move to the next cluster
        if (cluster >= 0x0FFFFFF8) break; // End of cluster chain
    }
    return 0; // File not found
}

// Read a specific number of clusters into the buffer
static void read_clusters(uint32_t start_cluster, void *buffer, size_t size) {
    uint32_t cluster_size = get_cluster_size();
    uint32_t clusters_to_read = (size + cluster_size - 1) / cluster_size; // Calculate how many clusters are needed

    for (uint32_t i = 0; i < clusters_to_read; i++) {
        uint32_t current_cluster = start_cluster;
        for (uint32_t j = 0; j < i; j++) {
            current_cluster = get_fat_entry(current_cluster);
        }
        uint32_t data_sector = data_start + ((current_cluster - 2) * boot_sector.sectors_per_cluster);
        ata_pio_read(data_sector, (uint8_t *)buffer + (i * cluster_size), cluster_size);
    }
}

// Allocate clusters for a new file
static uint32_t allocate_clusters(size_t size) {
    uint32_t cluster_size = get_cluster_size();
    uint32_t clusters_needed = (size + cluster_size - 1) / cluster_size; // Calculate needed clusters
    uint32_t start_cluster = 0; // Starting cluster for the new file
    uint32_t last_cluster = 0; // Last allocated cluster

    for (uint32_t i = 0; i < clusters_needed; i++) {
        // Find the next free cluster in the FAT
        uint32_t cluster = 2; // Start searching from cluster 2
        while (cluster < 0x0FFFFFF8) { // End of cluster chain
            if (get_fat_entry(cluster) == 0) { // Free cluster found
                if (start_cluster == 0) {
                    start_cluster = cluster; // First cluster allocated
                }
                if (last_cluster != 0) {
                    set_fat_entry(last_cluster, cluster); // Link the previous cluster to this one
                }
                last_cluster = cluster; // Update last allocated cluster
                set_fat_entry(cluster, 0x0FFFFFF8); // Mark cluster as end of chain
                break;
            }
            cluster++;
        }
        if (cluster >= 0x0FFFFFF8) {
            // No free clusters available
            return 0;
        }
    }

    return start_cluster; // Return the starting cluster
}

// Write clusters to the FAT32 filesystem
static void write_clusters(uint32_t start_cluster, const void *buffer, size_t size) {
    uint32_t cluster_size = get_cluster_size();
    uint32_t clusters_to_write = (size + cluster_size - 1) / cluster_size; // Calculate how many clusters are needed

    for (uint32_t i = 0; i < clusters_to_write; i++) {
        uint32_t current_cluster = start_cluster;
        for (uint32_t j = 0; j < i; j++) {
            current_cluster = get_fat_entry(current_cluster);
        }
        uint32_t data_sector = data_start + ((current_cluster - 2) * boot_sector.sectors_per_cluster);
        ata_pio_write(data_sector, (uint8_t *)buffer + (i * cluster_size), cluster_size);
    }
}

// Update a directory entry for the specified filename
static void update_directory_entry(const char *filename, uint32_t start_cluster, size_t size) {
    uint32_t cluster = boot_sector.root_cluster;
    uint32_t entry_offset = 0;

    while (true) {
        uint32_t cluster_size = get_cluster_size();
        uint8_t buffer[cluster_size];
        read_clusters(cluster, buffer, cluster_size);

        for (size_t i = 0; i < cluster_size; i += FAT32_ENTRY_SIZE) {
            // Read the directory entry
            if (buffer[i] == 0x00) { // End of directory entries
                return; // File not found
            }
            if (buffer[i] == 0xE5 || buffer[i] == 0x00) continue; // Deleted or empty entry

            char entry_name[12];
            kmemcpy(entry_name, buffer + i, 11);
            entry_name[11] = '\0'; // Null-terminate the string

            if (my_strcmp(entry_name, filename) == 0) {
                // Update the directory entry with file information
                *((uint32_t *)(buffer + i + 26)) = start_cluster; // Update starting cluster
                *((uint32_t *)(buffer + i + 28)) = size; // Update file size
                ata_pio_write(cluster, buffer, cluster_size); // Write back the updated directory entry
                return;
            }
        }

        cluster = get_fat_entry(cluster); // Move to the next cluster
        if (cluster >= 0x0FFFFFF8) break; // End of cluster chain
    }
}

// Read a file from the FAT32 filesystem
bool fat32_read_file(const char *filename, void *buffer, size_t size) {
    uint32_t start_cluster = find_file(filename);
    if (start_cluster == 0) {
        return false; // File not found
    }

    read_clusters(start_cluster, buffer, size); // Read file data into the buffer
    return true;
}

// Write a file to the FAT32 filesystem
bool fat32_write_file(const char *filename, const void *buffer, size_t size) {
    uint32_t start_cluster = allocate_clusters(size); // Allocate clusters for the file
    if (start_cluster == 0) {
        return false; // No free clusters available
    }

    write_clusters(start_cluster, buffer, size); // Write data to the allocated clusters
    update_directory_entry(filename, start_cluster, size); // Update the directory entry with file info
    return true;
}

bool fat32_delete_file(const char *filename) {
    uint32_t cluster = boot_sector.root_cluster;
    uint32_t entry_offset = 0;

    while (true) {
        uint32_t cluster_size = get_cluster_size();
        uint8_t buffer[cluster_size];
        read_clusters(cluster, buffer, cluster_size);

        for (size_t i = 0; i < cluster_size; i += FAT32_ENTRY_SIZE) {
            // Read the directory entry
            if (buffer[i] == 0x00) { // End of directory entries
                return false; // File not found
            }
            if (buffer[i] == 0xE5 || buffer[i] == 0x00) continue; // Deleted or empty entry

            char entry_name[12];
            kmemcpy(entry_name, buffer + i, 11);
            entry_name[11] = '\0'; // Null-terminate the string

            if (my_strcmp(entry_name, filename) == 0) {
                // Mark the file as deleted
                buffer[i] = 0xE5; // Set the first byte to 0xE5

                // Clear the FAT entries for the file
                uint32_t start_cluster = *((uint32_t *)(buffer + i + 26)); // Get starting cluster
                while (start_cluster < 0x0FFFFFF8) { // Traverse the cluster chain
                    uint32_t next_cluster = get_fat_entry(start_cluster); // Get the next cluster
                    set_fat_entry(start_cluster, 0); // Mark this cluster as free
                    start_cluster = next_cluster; // Move to the next cluster
                }

                // Write the updated directory entry back
                ata_pio_write(cluster, buffer, cluster_size);
                return true; // File deleted successfully
            }
        }

        cluster = get_fat_entry(cluster); // Move to the next cluster
        if (cluster >= 0x0FFFFFF8) break; // End of cluster chain
    }
    return false; // File not found
}
