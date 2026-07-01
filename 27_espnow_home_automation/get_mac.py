# Helper: Get MAC Address of your ESP32 board
# Run this on any ESP32 board to get its MAC address for ESP-NOW setup

import network

sta = network.WLAN(network.STA_IF)
sta.active(True)

mac = sta.config('mac')
mac_str = ":".join(["{:02X}".format(b) for b in mac])

print("MAC Address:", mac_str)
print()
print("For Python code, use:")
mac_bytes = "b'" + "".join(["\\x{:02x}".format(b) for b in mac]) + "'"
print("RECEIVER_MAC =", mac_bytes)
