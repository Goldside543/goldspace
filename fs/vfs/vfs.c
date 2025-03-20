// SPDX-License-Identifier: GPL-2.0-only
/*
 * fs/vfs/vfs.c
 *
 * Simple virtual file system layer I'm implementing.
 *
 * Copyright (C) 202402025 Goldside543
 *
 */

#include <stddef.h>
#include "../../kernel/string.h"

typedef struct FileSystem {
    const char *name;
    int (*mount)(const char *device);
    int (*unmount)();
    int (*open)(const char *path, int flags);
    int (*close)(int fd);
    int (*read)(int fd, void *buf, size_t size);
    int (*write)(int fd, const void *buf, size_t size);
} FileSystem;

typedef struct FileDescriptor {
    int fd;
    FileSystem *fs;
    void *private_data; // FS-specific file data (like inode pointer)
} FileDescriptor;

#define MAX_FILES 268435437
FileDescriptor open_files[MAX_FILES];

#define MAX_FS 4
FileSystem *registered_fs[MAX_FS];

void register_fs(FileSystem *fs) {
    for (int i = 0; i < MAX_FS; i++) {
        if (!registered_fs[i]) {
            registered_fs[i] = fs;
            return;
        }
    }
}

int vfs_mount(const char *fs_name, const char *device, void* unused1, void* unused2) {
    for (int i = 0; i < MAX_FS; i++) {
        if (registered_fs[i] && my_strcmp(registered_fs[i]->name, fs_name) == 0) {
            return registered_fs[i]->mount(device);
        }
    }
    return -1; // FS not found
}

int vfs_open(const char *path, int flags, void* unused1, void* unused2) {
    for (int i = 0; i < MAX_FS; i++) {
        if (registered_fs[i]) {
            int fd = registered_fs[i]->open(path, flags);
            if (fd >= 0) {
                open_files[fd].fs = registered_fs[i];
                return fd;
            }
        }
    }
    return -1; // File not found
}

ssize_t vfs_read(int fd, void *buf, size_t size, void* unused1) {
    if (fd < 0 || fd >= MAX_FILES || !open_files[fd].fs) return -1;
    return open_files[fd].fs->read(fd, buf, size);
}

ssize_t vfs_write(int fd, const void *buf, size_t size, void* unused 1) {
    if (fd < 0 || fd >= MAX_FILES || !open_files[fd].fs) return -1;
    return open_files[fd].fs->write(fd, buf, size);
}

int vfs_close(int fd, void* unused1, void* unused2, void* unused3) {
    if (fd < 0 || fd >= MAX_FILES || !open_files[fd].fs) return -1;
    open_files[fd].fs->close(fd);
    open_files[fd].fs = NULL;
    return 0;
}
