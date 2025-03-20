// SPDX-License-Identifier: GPL-2.0-only
/*
 * fs/ramfs/ramfs.c
 *
 * RAM file system.
 *
 * Copyright (C) 2025 Goldside543
 *
 */

#include "../../mm/memory.h"
#include "../../kernel/string.h"

struct ramfs_node *root;  // Root directory of RAMFS

void ramfs_init() {
    root = kmalloc(sizeof(struct ramfs_node));
    my_strcpy(root->name, "/");
    root->is_directory = 1;
    root->parent = NULL;
    root->children = NULL;
    root->next = NULL;
}

// Find a file in a directory
struct ramfs_node *ramfs_find(struct ramfs_node *dir, const char *name) {
    struct ramfs_node *node = dir->children;
    while (node) {
        if (my_strcmp(node->name, name) == 0) {
            return node;
        }
        node = node->next;
    }
    return NULL;
}

// Create or open a file
struct ramfs_node *ramfs_open(const char *path) {
    // Assume path parsing is done and `parent_dir` is found
    struct ramfs_node *parent_dir = root;
    struct ramfs_node *file = ramfs_find(parent_dir, path);
    
    if (!file) {  // Create if it doesn't exist
        file = kmalloc(sizeof(struct ramfs_node));
        my_strcpy(file->name, path);
        file->is_directory = 0;
        file->data = NULL;
        file->size = 0;
        file->parent = parent_dir;
        file->next = parent_dir->children;
        parent_dir->children = file;
    }
    return file;
}

// Write to a file
void ramfs_write(struct ramfs_node *file, const char *data, size_t size) {
    if (!file || file->is_directory) return;
    
    file->data = krealloc(file->data, size);
    kmemcpy(file->data, data, size);
    file->size = size;
}

// Read from a file
size_t ramfs_read(struct ramfs_node *file, char *buffer, size_t size) {
    if (!file || file->is_directory) return 0;
    
    size_t bytes_to_read = (size > file->size) ? file->size : size;
    kmemcpy(buffer, file->data, bytes_to_read);
    return bytes_to_read;
}

// Delete a file
void ramfs_unlink(struct ramfs_node *file) {
    if (!file || file->is_directory) return;

    if (file->data) kfree(file->data);
    
    // Remove from parent's child list
    struct ramfs_node **prev = &file->parent->children;
    while (*prev && *prev != file) {
        prev = &(*prev)->next;
    }
    if (*prev) *prev = file->next;

    kfree(file);
}
