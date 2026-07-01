# Project 27: ESP-NOW Home Automation - Sender (Shrike Fi / ESP32-S3)
# MicroPython - 4 buttons to control remote relays + OLED to show temp/humidity
#
# Wiring (Shrike Fi):
#   Button 1 -> ESP_IO1 (pullup, active low)
#   Button 2 -> ESP_IO2 (pullup, active low)
#   Button 3 -> ESP_IO3 (pullup, active low)
#   Button 4 -> ESP_IO4 (pullup, active low)
#   OLED SDA -> ESP_IO6
#   OLED SCL -> ESP_IO5

import network
import espnow
import time
from machine import Pin, I2C, SoftI2C
import ssd1306

# ===== CONFIGURE THIS =====
RECEIVER_MAC = b'\xFF\xFF\xFF\xFF\xFF\xFF'  # REPLACE with receiver MAC
# ============================

# Setup Wi-Fi (required for ESP-NOW)
sta = network.WLAN(network.STA_IF)
sta.active(True)
sta.disconnect()

# Setup ESP-NOW
e = espnow.ESPNow()
e.active(True)
e.add_peer(RECEIVER_MAC)

# Setup buttons (active low with internal pullup)
buttons = [
    Pin(1, Pin.IN, Pin.PULL_UP),
    Pin(2, Pin.IN, Pin.PULL_UP),
    Pin(3, Pin.IN, Pin.PULL_UP),
    Pin(4, Pin.IN, Pin.PULL_UP),
]

# Relay state tracking
relay_states = [0, 0, 0, 0]

# Setup OLED
i2c = SoftI2C(scl=Pin(5), sda=Pin(6), freq=400000)
oled = ssd1306.SSD1306_I2C(128, 64, i2c)

# Display vars
temp = "--"
hum  = "--"

last_press = [0, 0, 0, 0]


def update_display():
    oled.fill(0)
    oled.text("ESP-NOW Control", 5, 0)

    # Relay states
    for i in range(4):
        state = "ON " if relay_states[i] else "OFF"
        oled.text("R{}: {}".format(i + 1, state), 0, 16 + i * 10)

    # Temp/Humidity from receiver
    oled.text("T:{}C H:{}%".format(temp, hum), 0, 56)
    oled.show()


def check_buttons():
    for i in range(4):
        if buttons[i].value() == 0:  # pressed (active low)
            now = time.ticks_ms()
            if time.ticks_diff(now, last_press[i]) > 300:  # debounce
                last_press[i] = now
                relay_states[i] = 1 - relay_states[i]  # toggle
                msg = "{}:{}".format(i + 1, relay_states[i])
                e.send(RECEIVER_MAC, msg)
                print("Sent:", msg)
                update_display()


def check_incoming():
    global temp, hum
    host, msg = e.recv(0)  # non-blocking
    if msg:
        msg_str = msg.decode()
        if msg_str.startswith("T:"):
            # format: T:25.3,H:60.1
            try:
                parts = msg_str.split(",")
                temp = parts[0][2:]
                hum  = parts[1][2:]
                update_display()
                print("Sensor data: T={}C H={}%".format(temp, hum))
            except:
                pass


print("=== ESP-NOW Home Automation - Sender (Shrike Fi) ===")
print("Press buttons to toggle relays on receiver")
update_display()

while True:
    check_buttons()
    check_incoming()
    time.sleep_ms(50)
