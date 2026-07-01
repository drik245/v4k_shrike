# Project 23: OTA Update via GitHub - Shrike Fi (ESP32-S3)
# main.py - Sample user application (this file gets replaced by OTA updates)

from machine import Pin
import time

VERSION = "1.0.0"

led = Pin(21, Pin.OUT)

print("=== Shrike Fi OTA Demo ===")
print("Running version:", VERSION)
print("This code was loaded via OTA from GitHub!")
print("Blinking LED on ESP_IO21...")

while True:
    led.value(not led.value())
    time.sleep_ms(500)
