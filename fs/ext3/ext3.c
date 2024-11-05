// SPDX-License-Identifier: GPL-2.0-only
/*
 * fs/ext3/ext3.c
 *
 * Amazingly awful ext3 implementation.
 *
 * Copyright (C) 2024 Goldside543
 *
 */

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "../../kernel/string.h"
#include "../../mm/memory.h"
#include "../../drivers/disk.h"
#include "../../drivers/rtc.h"

// ext3 Superblock structure
typedef struct {
    uint32_t s_inodes_count;      // Total number of inodes
    uint32_t s_blocks_count;      // Total number of blocks
    uint32_t s_r_blocks_count;    // Reserved blocks count
    uint32_t s_free_blocks_count; // Free blocks count
    uint32_t s_free_inodes_count; // Free inodes count
    uint32_t s_first_data_block;  // First Data Block
    uint32_t s_log_block_size;    // Log2 of block size
    uint32_t s_log_frag_size;     // Log2 of fragment size
    uint32_t s_blocks_per_group;  // Blocks per group
    uint32_t s_frags_per_group;   // Fragments per group
    uint32_t s_inodes_per_group;  // Inodes per group
    uint32_t s_mtime;             // Mount time
    uint32_t s_wtime;             // Write time
    uint16_t s_mnt_count;         // Mount count
    uint16_t s_max_mnt_count;     // Max mount count before a check
    uint16_t s_magic;             // Magic number for filesystem identification
    uint16_t s_state;             // Filesystem state (clean, errors, etc.)
    uint16_t s_errors;            // Error behavior (continue, remount read-only, panic)
    uint16_t s_minor_rev_level;   // Minor revision level
    uint32_t s_lastcheck;         // Time of last filesystem check
    uint32_t s_checkinterval;     // Max time between checks
    uint32_t s_creator_os;        // OS that created the filesystem
    uint32_t s_rev_level;         // Revision level
    uint16_t s_def_resuid;        // Default UID for reserved blocks
    uint16_t s_def_resgid;        // Default GID for reserved blocks
    uint32_t s_first_ino;         // First non-reserved inode
    uint16_t s_inode_size;        // Size of inode structure
    uint16_t s_block_group_nr;    // Block group number
    uint32_t s_feature_compat;    // Compatible features
    uint32_t s_feature_incompat;  // Incompatible features
    uint32_t s_feature_ro_compat; // Read-only compatible features
    uint8_t  s_uuid[16];          // UUID for filesystem identification
    uint8_t  s_volume_name[16];   // Volume name
    uint8_t  s_last_mounted[64];  // Last mounted directory
    uint32_t s_algo_bitmap;       // Compression algorithm bitmap
    uint8_t  s_prealloc_blocks;    // Preallocation of blocks
    uint8_t  s_prealloc_dir_blocks; // Preallocation of directory blocks
    uint16_t s_padding1;           // Padding for alignment
    uint8_t  s_reserved[12];       // Reserved for future use
    uint32_t s_journal_uuid[4];   // UUID of the journal
    uint32_t s_journal_inum;       // Inode number of the journal file
    uint32_t s_journal_dev;        // Device number of the journal file
    uint32_t s_last_orphan;        // Last orphan inode
    uint32_t s_hash_seed[4];       // Hash seed for the filesystem
    uint8_t  s_def_hash_version;   // Default hash version
    uint8_t  s_reserved2[3];       // Reserved
    uint32_t s_reserved3;          // Reserved for future use
} __attribute__((packed)) ext3_superblock_t;

// ext3 Inode structure
typedef struct {
    uint16_t i_mode;          // File mode (type, permissions)
    uint16_t i_uid;           // Owner UID
    uint32_t i_size;          // Size in bytes
    uint32_t i_atime;         // Last access time
    uint32_t i_ctime;         // Creation time
    uint32_t i_mtime;         // Last modification time
    uint32_t i_dtime;         // Deletion time
    uint16_t i_gid;           // Group ID
    uint16_t i_links_count;   // Number of hard links
    uint32_t i_blocks;        // Blocks allocated to the file
    uint32_t i_flags;         // File flags (e.g., immutable)
    uint32_t i_osd1;          // OS-dependent value
    uint32_t i_block[15];     // Pointers to blocks (direct + indirect)
    uint32_t i_generation;     // File version (for NFS)
    uint32_t i_file_acl;      // Access Control List
    uint32_t i_dir_acl;       // Directory Access Control List
    uint32_t i_faddr;         // Fragment address
    uint8_t  i_frag[4];       // Fragment information
    uint8_t  i_osd2[12];      // OS-dependent values
} __attribute__((packed)) ext3_inode_t;

// ext3 Block Group Descriptor structure
typedef struct {
    uint32_t bg_block_bitmap;       // Block bitmap
    uint32_t bg_inode_bitmap;        // Inode bitmap
    uint32_t bg_inode_table;         // Inode table block
    uint16_t bg_free_blocks_count;   // Free blocks count
    uint16_t bg_free_inodes_count;   // Free inodes count
    uint16_t bg_used_dirs_count;     // Count of used directories
    uint16_t bg_pad;                 // Padding
    uint32_t bg_reserved[3];         // Reserved for future use
} __attribute__((packed)) ext3_group_desc_t;

// Global variables
ext3_superblock_t *superblock;
ext3_group_desc_t *group_desc;

// Function to read the superblock
bool read_superblock(uint32_t lba) {
    superblock = (ext3_superblock_t *)kmalloc(sizeof(ext3_superblock_t));
    if (!superblock) return false; // Memory allocation failed
    if (ata_pio_read(lba + 2, superblock, sizeof(ext3_superblock_t)) != sizeof(ext3_superblock_t)) {
        kfree(superblock);
        return false; // Reading failed
    }
    return true; // Successfully read superblock
}

// Function to read an inode
bool read_inode(uint32_t inode_number, ext3_inode_t *inode) {
    uint32_t block_size = 1024 << superblock->s_log_block_size; // Calculate block size
    uint32_t inode_table_block = group_desc->bg_inode_table; // Get inode table block
    uint32_t inode_size = superblock->s_inode_size; // Size of each inode
    uint32_t inode_index = inode_number - 1; // Inodes are 1-based
    uint32_t block_index = inode_index * inode_size / block_size; // Determine block index
    uint32_t offset = (inode_index * inode_size) % block_size; // Calculate offset within block

    // Read the inode from the appropriate block
    if (ata_pio_read(inode_table_block + block_index, inode, inode_size) != inode_size) {
        return false; // Reading failed
    }

    return true; // Successfully read inode
}

// Function to write an inode
bool write_inode(uint32_t inode_number, const ext3_inode_t *inode) {
    uint32_t block_size = 1024 << superblock->s_log_block_size; // Calculate block size
    uint32_t inode_table_block = group_desc->bg_inode_table; // Get inode table block
    uint32_t inode_size = superblock->s_inode_size; // Size of each inode
    uint32_t inode_index = inode_number - 1; // Inodes are 1-based
    uint32_t block_index = inode_index * inode_size / block_size; // Determine block index
    uint32_t offset = (inode_index * inode_size) % block_size; // Calculate offset within block

    // Write the inode to the appropriate block
    if (ata_pio_write(inode_table_block + block_index, inode, inode_size) != inode_size) {
        return false; // Writing failed
    }

    return true; // Successfully wrote inode
}

// Function to read the block group descriptors
bool read_group_descriptors(uint32_t lba) {
    uint32_t group_count = (superblock->s_blocks_count + superblock->s_blocks_per_group - 1) / superblock->s_blocks_per_group; // Calculate number of groups
    group_desc = (ext3_group_desc_t *)kmalloc(group_count * sizeof(ext3_group_desc_t)); // Allocate memory for group descriptors
    
    // Read group descriptors from disk
    if (!group_desc || ata_pio_read(lba + 2 + 1, group_desc, group_count * sizeof(ext3_group_desc_t)) != group_count * sizeof(ext3_group_desc_t)) {
        kfree(group_desc);
        return false; // Reading failed
    }

    return true; // Successfully read group descriptors
}

// Function to write the superblock
bool write_superblock() {
    if (ata_pio_write(2, superblock, sizeof(ext3_superblock_t)) != sizeof(ext3_superblock_t)) {
        return false; // Writing failed
    }
    return true; // Successfully wrote superblock
}

// Function to write the group descriptors
bool write_group_descriptors() {
    uint32_t group_count = (superblock->s_blocks_count + superblock->s_blocks_per_group - 1) / superblock->s_blocks_per_group; // Calculate number of groups
    if (ata_pio_write(2 + 1, group_desc, group_count * sizeof(ext3_group_desc_t)) != group_count * sizeof(ext3_group_desc_t)) {
        return false; // Writing failed
    }
    return true; // Successfully wrote group descriptors
}

// Function to write data to a file
bool write_file(const char *path, const void *data, size_t size) {
    ext3_inode_t inode;
    // Retrieve inode for the file
    if (!lookup_inode(path, &inode)) {
        return false; // File not found
    }

    // Variables to hold the current time
    uint8_t hour, minute, second;
    // Get the current time from RTC
    get_current_time(&hour, &minute, &second);
    
    // Update the inode's size and modification time
    inode.i_size = size;
    inode.i_mtime = second + (minute << 8) + (hour << 16); // Assuming inode.mtime expects a timestamp format

    // Write the inode back to disk
    if (!write_inode(inode_number, &inode)) {
        return false; // Writing inode failed
    }

    // Write the actual data
    uint32_t block_size = 1024 << superblock->s_log_block_size; // Calculate block size
    uint32_t block_index = inode.i_block[0]; // Assume single block for simplicity

    // Write data to the block
    if (ata_pio_write(block_index, data, size) != size) {
        return false; // Writing data failed
    }

    return true; // Successfully wrote file
}

// Function to initialize the ext3 filesystem
bool ext3_init(uint32_t lba) {
    // Read the superblock
    if (!read_superblock(lba)) {
        return false; // Superblock read failed
    }

    // Read the group descriptors
    if (!read_group_descriptors(lba)) {
        kfree(superblock);
        return false; // Group descriptor read failed
    }

    return true; // Initialization successful
}
