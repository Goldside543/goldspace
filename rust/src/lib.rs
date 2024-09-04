// SPDX-License-Identifier: GPL-2.0-only
/*!
 * src/lib.rs
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

pub fn log_message(message: &str) {
    unsafe {
        for byte in message.as_bytes() {
            if LOG_INDEX < LOG_BUFFER.len() {
                LOG_BUFFER[LOG_INDEX] = *byte;
                LOG_INDEX += 1;
            }
        }
    }
}

pub fn get_log_buffer() -> &'static [u8] {
    unsafe { &LOG_BUFFER[..LOG_INDEX] }
}
