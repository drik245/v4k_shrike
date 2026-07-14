"""
Project 49: Beating Heart on 8x8 LED Matrix (Shrike Lite - RP2040)

Displays a beating heart animation on a raw 8x8 LED matrix display,
driven entirely by GPIO pins (no external driver IC required).

Wiring:
  ROW 1..8 -> RP_IO16, 17, 18, 19, 20, 21, 22, 23 (Don't forget current-limiting resistors!)
  COL 1..8 -> RP_IO24, 25, 26, 27, 28, 29, 5, 6
"""

import time
from matrix import Matrix8x8
from icons import ICONS

# Define pins for Shrike Lite
ROW_PINS = [15, 8, 16, 10, 23, 17, 22, 19]
COL_PINS = [9, 21, 20, 14, 18, 11, 7, 6]

# Initialize matrix
# NOTE: Set common_anode=True if your matrix is common anode!
m = Matrix8x8(ROW_PINS, COL_PINS, common_anode=False, frame_rate=50)

# Get the big heart from the icons library
big_heart = ICONS['heart']

# Create a smaller heart for the beating effect
small_heart = bytes([
    0b00000000,
    0b00100100,
    0b01111110,
    0b01111110,
    0b00111100,
    0b00011000,
    0b00000000,
    0b00000000
])

# Create an even smaller tiny heart!
tiny_heart = bytes([
    0b00000000,
    0b00000000,
    0b00000000,
    0b00100100,
    0b00011000,
    0b00000000,
    0b00000000,
    0b00000000
])

print("Beating heart started. Press Ctrl+C to stop.")

try:
    while True:
        m.draw_bitmap(big_heart)
        time.sleep_ms(300)
        m.draw_bitmap(small_heart)
        time.sleep_ms(150)
        m.draw_bitmap(tiny_heart)
        time.sleep_ms(150)
        m.draw_bitmap(small_heart)
        time.sleep_ms(150)
except KeyboardInterrupt:
    m.stop()
    print("Stopped.")
