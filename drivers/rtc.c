// SPDX-License-Identifier: GPL-2.0-only
/*
 * drivers/rtc.c
 * 
 * RTC driver for telling time.
 *
 * Copyright (C) 2024-2025 Goldside543
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

uint32_t rtc_to_unix(int year, int month, int day, int hour, int min, int sec) {
    static const int days_in_month[] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
    uint32_t days = 0;

    // Count days in previous years
    for (int y = 1970; y < year; y++) {
        days += (y % 4 == 0 && (y % 100 != 0 || y % 400 == 0)) ? 366 : 365;
    }

    // Count days in the current year
    for (int m = 1; m < month; m++) {
        days += days_in_month[m - 1];
        if (m == 2 && (year % 4 == 0 && (year % 100 != 0 || year % 400 == 0))) {
            days++; // Leap year adjustment
        }
    }

    days += day - 1; // Add days in the current month

    return ((days * 86400) + (hour * 3600) + (min * 60) + sec);
}

// Get current time from RTC
uint32_t read_rtc_unix_time() {
    uint8_t second, minute, hour, day, month, year;
    
    // Read RTC registers
    second = bcd_to_bin(rtc_read(0x00));
    minute = bcd_to_bin(rtc_read(0x02));
    hour   = bcd_to_bin(rtc_read(0x04));
    day    = bcd_to_bin(rtc_read(0x07));
    month  = bcd_to_bin(rtc_read(0x08));
    year   = bcd_to_bin(rtc_read(0x09));

    // Adjust for century (assuming it's 20XX)
    year += 2000;

    // Convert to Unix timestamp
    return rtc_to_unix(year, month, day, hour, minute, second);
}
