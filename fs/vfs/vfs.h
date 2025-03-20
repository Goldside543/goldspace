/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef VFS_H
#define VFS_H

#include <stddef.h>

void register_fs(FileSystem *fs);

int vfs_mount(const char *fs_name, const char *device);

int vfs_open(const char *path, int flags);

ssize_t vfs_read(int fd, void *buf, size_t size);

ssize_t vfs_write(int fd, const void *buf, size_t size);

int vfs_close(int fd);

#endif // VFS_H
