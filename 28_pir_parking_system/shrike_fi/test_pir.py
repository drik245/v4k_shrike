"""
PIR Test Script (Shrike Fi)
Monitors raw digital signals from two PIR sensors.
PIR 1 -> ESP_IO14
PIR 2 -> ESP_IO2
"""
import time
from machine import Pin

pir1 = Pin(14, Pin.IN, Pin.PULL_DOWN)
pir2 = Pin(2, Pin.IN, Pin.PULL_DOWN)

print("Starting PIR Test (Shrike Fi)...")

while True:
    val1 = pir1.value()
    val2 = pir2.value()
    print("PIR 1 (Pin 14): {} | PIR 2 (Pin 2): {}".format(val1, val2))
    time.sleep(0.5)
