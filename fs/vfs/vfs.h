/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef VFS_H
#define VFS_H

#include <stddef.h>

typedef int ssize_t;

struct stat {
    size_t st_size;  // File size in bytes
    unsigned int st_mode;  // File permissions (you can use `S_IFREG`, `S_IFDIR` for regular files, directories, etc.)
    unsigned int st_uid;   // User ID of file owner
    unsigned int st_gid;   // Group ID of file owner
};

typedef struct FileSystem {
    const char *name;
    int (*mount)(const char *device);
    int (*unmount)();
    int (*open)(const char *path, int flags);
    int (*close)(int fd);
    int (*read)(int fd, void *buf, size_t size);
    int (*write)(int fd, const void *buf, size_t size);
    int (*stat)(const char *path, struct stat *st);
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

int vfs_stat(const char *path, struct stat *st, void* unused2, void* unused3);

#endif // VFS_H
