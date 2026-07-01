# Project 28: PIR Parking System - Shrike Lite (RP2040)
# MicroPython - 2 PIR sensors monitor parking slots, OLED shows status
#
# Wiring (Shrike Lite):
#   PIR 1 OUT -> RP_IO16
#   PIR 2 OUT -> RP_IO17
#   OLED SDA  -> RP_IO6 (I2C1 SDA)
#   OLED SCL  -> RP_IO7 (I2C1 SCL)

import time
from machine import Pin, I2C
import ssd1306

# PIR sensor pins
pir1 = Pin(16, Pin.IN)
pir2 = Pin(17, Pin.IN)

# OLED setup (I2C1 on Shrike Lite)
i2c = I2C(1, scl=Pin(7), sda=Pin(6), freq=400000)
oled = ssd1306.SSD1306_I2C(128, 64, i2c)

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
