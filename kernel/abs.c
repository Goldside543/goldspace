// SPDX-License-Identifier: GPL-2.0-only
/*
 * kernel/abs.c
 *
 * Creates the abs function.
 *
 * Copyright (C) 2024 Goldside543
 *
 */

#include "abs.h"

// Function to compute the absolute value of an integer
int abs(int x) {
    return (x < 0) ? -x : x;
}
