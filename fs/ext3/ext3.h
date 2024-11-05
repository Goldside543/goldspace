/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef EXT3_H
#define EXT3_H

bool ext3_write_file(const char *path, const void *data, size_t size);
bool ext3_read_file(const char *path, void *buffer, size_t size, size_t *bytes_read);
bool ext3_init(uint32_t lba);

#endif // EXT3_H
