# Project 26: ESP-NOW RFID Display - Receiver (ESP32 WROOM)
# MicroPython - Receives RFID UID via ESP-NOW and displays on SSD1306 OLED
#
# Wiring (ESP32 WROOM):
#   OLED SDA -> GPIO 21
#   OLED SCL -> GPIO 22

import network
import espnow
import time
from machine import Pin, I2C
import ssd1306

# Setup I2C and OLED (ESP32 WROOM standard pins)
i2c = I2C(0, scl=Pin(22), sda=Pin(21), freq=400000)
oled = ssd1306.SSD1306_I2C(128, 64, i2c)

# Setup Wi-Fi in station mode (required for ESP-NOW)
sta = network.WLAN(network.STA_IF)
sta.active(True)
sta.disconnect()

# Print MAC address so sender can use it
mac = sta.config('mac')
mac_str = ":".join(["{:02X}".format(b) for b in mac])
print("=== ESP-NOW RFID Receiver - ESP32 WROOM ===")
print("My MAC address:", mac_str)
print("Paste this into sender.py as RECEIVER_MAC")

# Setup ESP-NOW
e = espnow.ESPNow()
e.active(True)

# Show ready screen
oled.fill(0)
oled.text("RFID Receiver", 10, 0)
oled.text("Waiting...", 25, 25)
oled.text("MAC:", 0, 50)
oled.text(mac_str[:17], 0, 58)
oled.show()

# Track scan history
scan_count = 0

while True:
    host, msg = e.recv()
    if msg:
        msg_str = msg.decode()
        print("Received:", msg_str)

        if msg_str.startswith("UID:"):
            uid = msg_str[4:]
            scan_count += 1

            oled.fill(0)
            oled.text("RFID Scanned!", 15, 0)
            oled.text("UID:", 0, 20)
            oled.text(uid, 0, 32)
            oled.text("Scan #{}".format(scan_count), 0, 50)
            oled.show()

            print("Displayed UID:", uid, "Scan #", scan_count)
