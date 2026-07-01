# Project 24: RFID Access Control - Shrike Fi (ESP32-S3)
# MicroPython - Scans RFID cards, checks UID against authorized list
#
# Wiring (SoftSPI on Shrike Fi):
#   MFRC522 SCK  -> ESP_IO5
#   MFRC522 MOSI -> ESP_IO6
#   MFRC522 MISO -> ESP_IO7
#   MFRC522 SDA  -> ESP_IO4 (CS)
#   MFRC522 RST  -> ESP_IO3
#   Green LED    -> ESP_IO1
#   Red LED      -> ESP_IO2

from machine import Pin
import time
from mfrc522 import MFRC522

# Pin setup
GREEN_LED = Pin(1, Pin.OUT)
RED_LED   = Pin(2, Pin.OUT)

GREEN_LED.value(0)
RED_LED.value(0)

# RFID reader setup (SoftSPI)
rfid = MFRC522(sck=5, mosi=6, miso=7, rst=3, cs=4)

# Authorized UIDs (add your card UIDs here after scanning them)
AUTHORIZED = [
    "AA:BB:CC:DD",
    "11:22:33:44",
]

print("=== RFID Access Control - Shrike Fi ===")
print("Scan a card to check access...")
print("Authorized UIDs:", AUTHORIZED)

last_uid = ""
last_time = 0

while True:
    (stat, tag_type) = rfid.request(rfid.REQIDL)

    if stat == rfid.OK:
        (stat, raw_uid) = rfid.anticoll()

        if stat == rfid.OK:
            uid_str = rfid.uid_to_str(raw_uid)

            # debounce: skip if same card scanned within 2 seconds
            now = time.ticks_ms()
            if uid_str == last_uid and time.ticks_diff(now, last_time) < 2000:
                continue
            last_uid = uid_str
            last_time = now

            print("Card UID:", uid_str)

            if uid_str in AUTHORIZED:
                print("ACCESS GRANTED")
                GREEN_LED.value(1)
                RED_LED.value(0)
                time.sleep(2)
                GREEN_LED.value(0)
            else:
                print("ACCESS DENIED")
                RED_LED.value(1)
                GREEN_LED.value(0)
                time.sleep(2)
                RED_LED.value(0)

    time.sleep_ms(100)
