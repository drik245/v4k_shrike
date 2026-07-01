# Project 23: OTA Update via GitHub - Shrike Fi (ESP32-S3)
# boot.py - Runs on every startup, triggers OTA check

import ota

print("=== Boot: Checking for OTA updates ===")
ota.check_for_update()
print("=== Boot: OTA check complete ===")
