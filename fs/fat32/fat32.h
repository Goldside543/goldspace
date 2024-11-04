/* SPDX-License-Identifier: GPL-2.0-only */

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#ifndef FAT32_H
#define FAT32_H

bool fat32_mount();
bool fat32_read_file(const char *filename, void *buffer, size_t size);
bool fat32_write_file(const char *filename, const void *buffer, size_t size);
static uint32_t get_cluster_size();
static uint32_t get_fat_entry(uint32_t cluster);
static void set_fat_entry(uint32_t cluster, uint32_t value);
static uint32_t find_file(const char *filename);
static void read_clusters(uint32_t start_cluster, void *buffer, size_t size);
static uint32_t allocate_clusters(size_t size);
static void write_clusters(uint32_t start_cluster, const void *buffer, size_t size);
static void update_directory_entry(const char *filename, uint32_t start_cluster, size_t size);
bool fat32_delete_file(const char *filename);

#endif // FAT32_H
