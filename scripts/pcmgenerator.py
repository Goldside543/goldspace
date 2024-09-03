# SPDX-License-Identifier: GPL-2.0-only

# Hello! This script generates a raw PCM file for testing audio!

import numpy as np
import wave

# Parameters
sampling_rate = 44100  # 44.1 kHz
duration = 1  # 1 second
frequency = 440  # 440 Hz (A4)

# Generate a sine wave
t = np.linspace(0, duration, int(sampling_rate * duration), endpoint=False)
waveform = 32767 * np.sin(2 * np.pi * frequency * t)  # 16-bit PCM

# Convert to 16-bit PCM
pcm_data = waveform.astype(np.int16).tobytes()

# Write to file
with wave.open('test_pcm.wav', 'wb') as wf:
    wf.setnchannels(1)  # Mono
    wf.setsampwidth(2)  # 16-bit
    wf.setframerate(sampling_rate)
    wf.writeframes(pcm_data)
