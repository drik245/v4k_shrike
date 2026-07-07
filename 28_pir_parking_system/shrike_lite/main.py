"""
Project 28: PIR Parking System (Shrike Lite)
Monitors 2 parking slots with PIR sensors.
Shows live status on OLED.

Wiring:
PIR 1        -> RP_IO16
PIR 2        -> RP_IO17
OLED SCK     -> RP_IO10
OLED MOSI    -> RP_IO11
OLED CS      -> RP_IO14
OLED DC      -> RP_IO15
OLED RES     -> RP_IO9
Dummy MISO   -> RP_IO8 (Keeps SPI from stealing other pins)
"""

import time
from machine import Pin, SPI
import ssd1306

# PIR sensor pins
pir1 = Pin(16, Pin.IN)
pir2 = Pin(17, Pin.IN)

"""
setup SPI OLED
we use a dummy MISO pin so the board doesn't 
secretly steal one of our other pins
"""
spi = SPI(1, baudrate=10000000, sck=Pin(10), mosi=Pin(11), miso=Pin(8))
oled_cs = Pin(14, Pin.OUT)
oled_dc = Pin(15, Pin.OUT)
oled_res = Pin(9, Pin.OUT)
oled = ssd1306.SSD1306_SPI(128, 64, spi, oled_dc, oled_res, oled_cs)

# Slot states (toggle on motion)
slot1_full = False
slot2_full = False

# Edge detection tracking
prev_pir1 = 0
prev_pir2 = 0

# Debounce
last_trigger1 = 0
last_trigger2 = 0
DEBOUNCE_MS = 3000


def update_display():
    oled.fill(0)
    oled.text("PARKING SYSTEM", 10, 0)
    oled.text("==============", 5, 10)

    s1 = "FULL" if slot1_full else "FREE"
    s2 = "FULL" if slot2_full else "FREE"

    oled.text("Slot 1: " + s1, 5, 25)
    if slot1_full:
        oled.fill_rect(100, 24, 20, 10, 1)
    else:
        oled.rect(100, 24, 20, 10, 1)

    oled.text("Slot 2: " + s2, 5, 40)
    if slot2_full:
        oled.fill_rect(100, 39, 20, 10, 1)
    else:
        oled.rect(100, 39, 20, 10, 1)

    total_free = (0 if slot1_full else 1) + (0 if slot2_full else 1)
    oled.text("Available: {}/2".format(total_free), 5, 55)
    oled.show()


print("=== PIR Parking System - Shrike Lite ===")
update_display()

while True:
    now = time.ticks_ms()

    curr_pir1 = pir1.value()
    if curr_pir1 == 1 and prev_pir1 == 0:
        if time.ticks_diff(now, last_trigger1) > DEBOUNCE_MS:
            slot1_full = not slot1_full
            last_trigger1 = now
            print("Slot 1:", "FULL" if slot1_full else "FREE")
            update_display()
    prev_pir1 = curr_pir1

    curr_pir2 = pir2.value()
    if curr_pir2 == 1 and prev_pir2 == 0:
        if time.ticks_diff(now, last_trigger2) > DEBOUNCE_MS:
            slot2_full = not slot2_full
            last_trigger2 = now
            print("Slot 2:", "FULL" if slot2_full else "FREE")
            update_display()
    prev_pir2 = curr_pir2

    time.sleep_ms(50)
