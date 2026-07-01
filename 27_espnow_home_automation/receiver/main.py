# Project 27: ESP-NOW Home Automation - Receiver (ESP32 WROOM)
# MicroPython - 4-channel relay + DHT22, receives commands from sender
#
# Wiring (ESP32 WROOM):
#   Relay 1  -> GPIO 16
#   Relay 2  -> GPIO 17
#   Relay 3  -> GPIO 18
#   Relay 4  -> GPIO 19
#   DHT22    -> GPIO 4

import network
import espnow
import time
import dht
from machine import Pin

# ===== CONFIGURE =====
ACTIVE_LOW_RELAY = True  # Most relay modules are active-low
SENSOR_INTERVAL  = 2000  # ms between DHT reads
# ======================

# Setup Wi-Fi (required for ESP-NOW)
sta = network.WLAN(network.STA_IF)
sta.active(True)
sta.disconnect()

# Print MAC so sender can use it
mac = sta.config('mac')
mac_str = ":".join(["{:02X}".format(b) for b in mac])
print("=== ESP-NOW Home Automation - Receiver (ESP32 WROOM) ===")
print("My MAC address:", mac_str)

# Setup ESP-NOW
e = espnow.ESPNow()
e.active(True)

# Track sender MAC for replies
sender_mac = None

# Relay pins
relay_pins = [
    Pin(16, Pin.OUT),
    Pin(17, Pin.OUT),
    Pin(18, Pin.OUT),
    Pin(19, Pin.OUT),
]

relay_states = [0, 0, 0, 0]

# Initialize relays to OFF
for rp in relay_pins:
    rp.value(1 if ACTIVE_LOW_RELAY else 0)

# DHT22 sensor
dht_sensor = dht.DHT22(Pin(4))

last_sensor_read = 0


def apply_relay(idx):
    state = relay_states[idx]
    if ACTIVE_LOW_RELAY:
        relay_pins[idx].value(0 if state else 1)
    else:
        relay_pins[idx].value(state)
    print("Relay {} -> {}".format(idx + 1, "ON" if state else "OFF"))


def read_and_send_sensor():
    global last_sensor_read, sender_mac
    now = time.ticks_ms()
    if time.ticks_diff(now, last_sensor_read) < SENSOR_INTERVAL:
        return
    last_sensor_read = now

    try:
        dht_sensor.measure()
        t = dht_sensor.temperature()
        h = dht_sensor.humidity()
        msg = "T:{:.1f},H:{:.1f}".format(t, h)
        print("Sensor:", msg)

        if sender_mac:
            try:
                e.add_peer(sender_mac)
            except:
                pass
            e.send(sender_mac, msg)
    except Exception as ex:
        print("DHT error:", ex)


print("Waiting for commands...")

while True:
    host, msg = e.recv(0)  # non-blocking
    if msg:
        sender_mac = host  # save sender MAC for replies
        msg_str = msg.decode()
        print("Received:", msg_str, "from", ":".join("{:02X}".format(b) for b in host))

        # parse "R:S" format (e.g. "1:1" = relay 1 ON)
        try:
            parts = msg_str.split(":")
            relay_num = int(parts[0]) - 1  # 0-indexed
            state = int(parts[1])
            if 0 <= relay_num < 4:
                relay_states[relay_num] = state
                apply_relay(relay_num)
        except:
            print("Invalid command:", msg_str)

    read_and_send_sensor()
    time.sleep_ms(50)
