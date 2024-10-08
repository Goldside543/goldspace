// SPDX-License-Identifier: GPL-2.0-only
/*
 * drivers/rtc.c
 * 
 * RTC driver for telling time.
 *
 * Copyright (C) 2024 Goldside543
 *
 */

#include "rtc.h"
#include "../kernel/io.h"

// Read a register from the RTC
uint8_t rtc_read(uint8_t reg) {
    outb(RTC_INDEX_PORT, reg);
    return inb(RTC_DATA_PORT);
}

// Convert BCD to binary
uint8_t bcd_to_bin(uint8_t bcd) {
    return (bcd >> 4) * 10 + (bcd & 0x0F);
}

// Get current time from RTC
void get_current_time(uint8_t *hour, uint8_t *minute, uint8_t *second) {
    *second = bcd_to_bin(rtc_read(RTC_SECONDS));
    *minute = bcd_to_bin(rtc_read(RTC_MINUTES));
    *hour = bcd_to_bin(rtc_read(RTC_HOURS));
}
