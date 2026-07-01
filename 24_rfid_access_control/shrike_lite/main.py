# Project 24: RFID Access Control - Shrike Lite (RP2040)
# MicroPython - Scans RFID cards, checks UID against authorized list
#
# Wiring (SoftSPI on Shrike Lite):
#   MFRC522 SCK  -> RP_IO18
#   MFRC522 MOSI -> RP_IO19
#   MFRC522 MISO -> RP_IO16
#   MFRC522 SDA  -> RP_IO17 (CS)
#   MFRC522 RST  -> RP_IO20
#   Green LED    -> RP_IO15
#   Red LED      -> RP_IO14

from machine import Pin
import time
from mfrc522 import MFRC522

# Pin setup
GREEN_LED = Pin(15, Pin.OUT)
RED_LED   = Pin(14, Pin.OUT)

GREEN_LED.value(0)
RED_LED.value(0)

# RFID reader setup (SoftSPI)
rfid = MFRC522(sck=18, mosi=19, miso=16, rst=20, cs=17)

# Authorized UIDs (add your card UIDs here after scanning them)
AUTHORIZED = [
    "AA:BB:CC:DD",
    "11:22:33:44",
]

print("=== RFID Access Control - Shrike Lite ===")
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
