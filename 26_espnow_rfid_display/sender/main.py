# Project 26: ESP-NOW RFID Display - Sender (Shrike Fi / ESP32-S3)
# MicroPython - Scans RFID card and sends UID via ESP-NOW to receiver
#
# Wiring (SoftSPI on Shrike Fi):
#   MFRC522 SCK  -> ESP_IO5
#   MFRC522 MOSI -> ESP_IO6
#   MFRC522 MISO -> ESP_IO7
#   MFRC522 SDA  -> ESP_IO4 (CS)
#   MFRC522 RST  -> ESP_IO3

import network
import espnow
import time
from mfrc522 import MFRC522

# ===== CONFIGURE THIS =====
# Run get_mac.py on the RECEIVER board first, then paste its MAC here
RECEIVER_MAC = b'\xFF\xFF\xFF\xFF\xFF\xFF'  # REPLACE with actual receiver MAC
# ============================

# Setup Wi-Fi in station mode (required for ESP-NOW)
sta = network.WLAN(network.STA_IF)
sta.active(True)
sta.disconnect()

# Setup ESP-NOW
e = espnow.ESPNow()
e.active(True)
e.add_peer(RECEIVER_MAC)

# RFID reader
rfid = MFRC522(sck=5, mosi=6, miso=7, rst=3, cs=4)

print("=== ESP-NOW RFID Sender - Shrike Fi ===")
print("Scan a card to send UID to receiver...")

last_uid = ""
last_time = 0

while True:
    (stat, tag_type) = rfid.request(rfid.REQIDL)

    if stat == rfid.OK:
        (stat, raw_uid) = rfid.anticoll()

        if stat == rfid.OK:
            uid_str = rfid.uid_to_str(raw_uid)

            # debounce
            now = time.ticks_ms()
            if uid_str == last_uid and time.ticks_diff(now, last_time) < 2000:
                continue
            last_uid = uid_str
            last_time = now

            print("Card scanned:", uid_str)
            msg = "UID:" + uid_str
            e.send(RECEIVER_MAC, msg)
            print("Sent to receiver:", msg)

    time.sleep_ms(100)
