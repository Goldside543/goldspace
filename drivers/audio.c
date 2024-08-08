// SPDX-License-Identifier: GPL-2.0-only
/*
 * drivers/audio.c
 *
 * Audio driver.
 *
 * Copyright (C) 2024 Goldside543
 *
 */

#include "audio.h"
#include "../kernel/print.h"
#include <stdint.h>

#define AUDIO_DEVICE_BASE 0xC0000000   // Hypothetical base address for the audio device
#define AUDIO_CTRL_REG    (AUDIO_DEVICE_BASE + 0x00) // Control register offset
#define AUDIO_DATA_REG    (AUDIO_DEVICE_BASE + 0x04) // Data register offset
#define AUDIO_STATUS_REG  (AUDIO_DEVICE_BASE + 0x08) // Status register offset

#define AUDIO_CTRL_ENABLE 0x01        // Control register bit to enable the audio
#define AUDIO_CTRL_RESET  0x02        // Control register bit to reset the audio

// Hypothetical audio device setup values
#define AUDIO_SAMPLE_RATE 44100       // 44.1 kHz
#define AUDIO_BIT_DEPTH    16         // 16-bit PCM
#define AUDIO_CHANNELS     1          // Mono

void audio_init(void) {
    print("Loading audio driver...\n");
    // Step 1: Reset the audio device
    volatile uint32_t *audio_ctrl = (volatile uint32_t *)AUDIO_CTRL_REG;
    volatile uint32_t *audio_status = (volatile uint32_t *)AUDIO_STATUS_REG;
    volatile uint32_t *audio_data = (volatile uint32_t *)AUDIO_DATA_REG;

    *audio_ctrl = AUDIO_CTRL_RESET; // Send reset command

    // Wait for the reset to complete with a timeout
    unsigned int timeout = 3000000; // Adjust timeout value as needed
    while (*audio_status & 0x01) {  // Check if reset bit is set
        if (--timeout == 0) {
            print("Audio device not found. Continuing boot without audio driver.\n");
            return; // Timeout occurred, exit the function
        }
    }

    // Step 2: Configure the audio device
    *audio_ctrl = 0; // Clear control register

    // Set sample rate, bit depth, and channels (hypothetical settings)
    // Note: Actual register configuration will vary by hardware
    *audio_data = AUDIO_SAMPLE_RATE; // Set sample rate

    // Example: Configure bit depth and channels
    *audio_data = AUDIO_BIT_DEPTH;   // Set bit depth
    *audio_data = AUDIO_CHANNELS;    // Set number of channels

    // Step 3: Enable the audio device
    *audio_ctrl = AUDIO_CTRL_ENABLE; // Enable the audio device
    print("Audio driver loaded.\n");
}
