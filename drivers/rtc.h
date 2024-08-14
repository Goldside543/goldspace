/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef RTC_H
#define RTC_H

#include <stdint.h>

// RTC I/O Ports
#define RTC_INDEX_PORT 0x70
#define RTC_DATA_PORT  0x71

// RTC Register Addresses
#define RTC_SECONDS    0x00
#define RTC_MINUTES    0x02
#define RTC_HOURS      0x04
#define RTC_DAY        0x07
#define RTC_MONTH      0x08
#define RTC_YEAR       0x09

uint8_t rtc_read(uint8_t reg);

uint8_t bcd_to_bin(uint8_t bcd);

void get_current_time(uint8_t *hour, uint8_t *minute, uint8_t *second);

#endif // RTC_H
