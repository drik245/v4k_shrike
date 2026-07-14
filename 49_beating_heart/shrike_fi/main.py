"""
Project 49: Beating Heart on 8x8 LED Matrix (Shrike Fi - ESP32-S3)

Displays a beating heart animation on a raw 8x8 LED matrix display,
driven entirely by GPIO pins (no external driver IC required).

Wiring:
  ROW 1..8 -> ESP_IO16, 17, 18, 19, 20, 21, 41, 42 (Don't forget current-limiting resistors!)
  COL 1..8 -> ESP_IO2, 1, 3, 46, 4, 5, 6, 7
"""

import time
from matrix import Matrix8x8
from icons import ICONS

# Define pins for Shrike Fi
ROW_PINS = [16, 17, 18, 19, 20, 21, 41, 42]
COL_PINS = [2, 1, 3, 46, 4, 5, 6, 7]

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
        # We can use the built-in animation player to loop indefinitely
        # by repeating the frames list many times, or just loop it manually:
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
