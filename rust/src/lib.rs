// SPDX-License-Identifier: GPL-2.0-only
/*!
 * rust/src/lib.rs
 *
 * Goldspace's Rust library, where Rust functions can
 * be used in the kernel. Seeing as Rust usage in the
 * OS world has been increasing, I decided "eh, why
 * not" and slapped Rust in.
 *
 * Copyright (C) 2024 Goldside543
 */

#![no_std]
#![no_main]
#![allow(warnings)]

use core::panic::PanicInfo;
use core::arch::asm;

static mut LOG_BUFFER: [u8; 1024] = [0; 1024];
static mut LOG_INDEX: usize = 0;

#[panic_handler]
#[no_mangle]
fn panic(_info: &PanicInfo) -> ! {
    // Halt the CPU
    loop {
        unsafe {
            asm!("hlt");
        }
    }
}

pub enum LogLevel {
    Info,
    Warn,
    Error,
}

#[no_mangle]
pub fn log_message(level: LogLevel, message: &str) {
    unsafe {
        let prefix: &[u8] = match level {
            LogLevel::Info => b"[INFO] ",
            LogLevel::Warn => b"[WARN] ",
            LogLevel::Error => b"[ERROR] ",
        };

        // Write the log level prefix
        for &byte in prefix {
            if LOG_INDEX < LOG_BUFFER.len() {
                LOG_BUFFER[LOG_INDEX] = byte;
                LOG_INDEX += 1;
            }
        }

        // Write the message
        for byte in message.as_bytes() {
            if LOG_INDEX < LOG_BUFFER.len() {
                LOG_BUFFER[LOG_INDEX] = *byte;
                LOG_INDEX += 1;
            }
        }

        // Add a newline after the message
        if LOG_INDEX < LOG_BUFFER.len() {
            LOG_BUFFER[LOG_INDEX] = b'\n';
            LOG_INDEX += 1;
        }
        
        // Handle buffer overflow
        if LOG_INDEX >= LOG_BUFFER.len() {
            LOG_INDEX = LOG_BUFFER.len() - 1; // Limit the index to prevent overflow
        }
    }
}

#[no_mangle]
pub fn get_log_buffer() -> &'static [u8] {
    unsafe { &LOG_BUFFER[..LOG_INDEX] }
}

// Helper function to get the current time as a string (example implementation)
#[no_mangle]
fn get_current_time() -> &'static str {
    "????-???-??? ??:??" // Placeholder for actual time retrieval
}

// Add timestamp to log messages
#[no_mangle]
pub fn log_message_with_timestamp(level: LogLevel, message: &str) {
    unsafe {
        let time_stamp = get_current_time();
        let prefix: &[u8] = match level {
            LogLevel::Info => b"[INFO] ",
            LogLevel::Warn => b"[WARN] ",
            LogLevel::Error => b"[ERROR] ",
        };

        // Write the timestamp
        for byte in time_stamp.as_bytes() {
            if LOG_INDEX < LOG_BUFFER.len() {
                LOG_BUFFER[LOG_INDEX] = *byte;
                LOG_INDEX += 1;
            }
        }
        
        // Write the log level prefix
        for &byte in prefix {
            if LOG_INDEX < LOG_BUFFER.len() {
                LOG_BUFFER[LOG_INDEX] = byte;
                LOG_INDEX += 1;
            }
        }

        // Write the message
        for byte in message.as_bytes() {
            if LOG_INDEX < LOG_BUFFER.len() {
                LOG_BUFFER[LOG_INDEX] = *byte;
                LOG_INDEX += 1;
            }
        }

        // Add a newline after the message
        if LOG_INDEX < LOG_BUFFER.len() {
            LOG_BUFFER[LOG_INDEX] = b'\n';
            LOG_INDEX += 1;
        }
        
        // Handle buffer overflow
        if LOG_INDEX >= LOG_BUFFER.len() {
            LOG_INDEX = LOG_BUFFER.len() - 1; // Limit the index to prevent overflow
        }
    }
}
