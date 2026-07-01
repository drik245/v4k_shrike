# Project 25: RFID Attendance Logger - Shrike Fi (ESP32-S3)
# MicroPython - Scans RFID, logs attendance to Google Sheets via webhook
#
# Wiring (same SPI as Project 24):
#   MFRC522 SCK  -> ESP_IO5
#   MFRC522 MOSI -> ESP_IO6
#   MFRC522 MISO -> ESP_IO7
#   MFRC522 SDA  -> ESP_IO4 (CS)
#   MFRC522 RST  -> ESP_IO3
#   Green LED    -> ESP_IO1
#   Red LED      -> ESP_IO2

import network
import urequests
import ntptime
import time
import ujson
from machine import Pin, RTC
from mfrc522 import MFRC522

# ===== CONFIGURE THESE =====
WIFI_SSID   = "YOUR_WIFI_SSID"
WIFI_PASS   = "YOUR_WIFI_PASSWORD"
WEBHOOK_URL = "https://script.google.com/macros/s/YOUR_SCRIPT_ID/exec"
# ============================

# Student database: UID -> Name
STUDENTS = {
    "AA:BB:CC:DD": "Student A",
    "11:22:33:44": "Student B",
}

# Track IN/OUT status per student
status_tracker = {}

# Pin setup
GREEN_LED = Pin(1, Pin.OUT)
RED_LED   = Pin(2, Pin.OUT)
GREEN_LED.value(0)
RED_LED.value(0)

# RFID reader
rfid = MFRC522(sck=5, mosi=6, miso=7, rst=3, cs=4)

rtc = RTC()


def connect_wifi():
    wlan = network.WLAN(network.STA_IF)
    wlan.active(True)
    if wlan.isconnected():
        return True
    print("Connecting to Wi-Fi:", WIFI_SSID)
    wlan.connect(WIFI_SSID, WIFI_PASS)
    timeout = 15
    while not wlan.isconnected() and timeout > 0:
        time.sleep(1)
        timeout -= 1
    if wlan.isconnected():
        print("Connected! IP:", wlan.ifconfig()[0])
        return True
    print("Wi-Fi failed")
    return False


def sync_time():
    try:
        ntptime.settime()
        print("NTP time synced")
    except:
        print("NTP sync failed, using default time")


def get_timestamp():
    t = rtc.datetime()
    return "{:04d}-{:02d}-{:02d} {:02d}:{:02d}:{:02d}".format(
        t[0], t[1], t[2], t[4], t[5], t[6]
    )


def log_to_sheets(name, status):
    timestamp = get_timestamp()
    data = {
        "timestamp": timestamp,
        "student": name,
        "status": status,
    }
    print("Logging:", data)
    try:
        response = urequests.post(
            WEBHOOK_URL,
            json=data,
            headers={"Content-Type": "application/json"}
        )
        print("Response:", response.status_code)
        response.close()
        return True
    except Exception as e:
        print("Failed to log:", e)
        return False


# Setup
print("=== RFID Attendance Logger - Shrike Fi ===")

if connect_wifi():
    sync_time()

print("Ready! Scan a card...")

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
            if uid_str == last_uid and time.ticks_diff(now, last_time) < 3000:
                continue
            last_uid = uid_str
            last_time = now

            if uid_str in STUDENTS:
                name = STUDENTS[uid_str]

                # toggle IN/OUT
                if uid_str not in status_tracker or status_tracker[uid_str] == "OUT":
                    status_tracker[uid_str] = "IN"
                else:
                    status_tracker[uid_str] = "OUT"

                status = status_tracker[uid_str]
                print("{} - {} ({})".format(name, status, uid_str))

                GREEN_LED.value(1)
                log_to_sheets(name, status)
                time.sleep(1)
                GREEN_LED.value(0)
            else:
                print("Unknown card:", uid_str)
                RED_LED.value(1)
                time.sleep(1)
                RED_LED.value(0)

    time.sleep_ms(100)
