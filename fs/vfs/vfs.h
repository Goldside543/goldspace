/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef VFS_H
#define VFS_H

int vfs_create_file(const char* name);
int vfs_write_file(int file_index, const char* data, size_t size);
int vfs_read_file(int file_index, char* buffer, size_t size);
int vfs_delete_file(int file_index);

#endif // VFS_H
