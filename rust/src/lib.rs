#![no_std]
#![no_main]

use core::panic::PanicInfo;
use core::arch::asm;

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
