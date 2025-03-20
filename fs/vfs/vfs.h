/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef VFS_H
#define VFS_H

#include <stddef.h>

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

void register_fs(FileSystem *fs);

int vfs_mount(const char *fs_name, const char *device, void* unused1, void* unused2);

int vfs_open(const char *path, int flags, void* unused1, void* unused2);

ssize_t vfs_read(int fd, void *buf, size_t size, void* unused1);

ssize_t vfs_write(int fd, const void *buf, size_t size, void* unused1);

int vfs_close(int fd, void* unused1, void* unused2, void* unused3);

#endif // VFS_H
