"""
PIR Test Script (Shrike Lite)
Monitors raw digital signals from two PIR sensors.
PIR 1 -> RP_IO16
PIR 2 -> RP_IO17
"""
import time
from machine import Pin

pir1 = Pin(16, Pin.IN, Pin.PULL_DOWN)
pir2 = Pin(17, Pin.IN, Pin.PULL_DOWN)

print("Starting PIR Test (Shrike Lite)...")

while True:
    val1 = pir1.value()
    val2 = pir2.value()
    print("PIR 1 (Pin 16): {} | PIR 2 (Pin 17): {}".format(val1, val2))
    time.sleep(0.5)
