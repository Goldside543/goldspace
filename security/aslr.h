/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef ASLR_H
#define ASLR_H

#include <stdint.h>

void init_random_seed();
uint32_t generate_random_address();

#endif // ASLR_H
