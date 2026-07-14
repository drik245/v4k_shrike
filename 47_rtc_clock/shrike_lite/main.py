"""
Project 47: RTC Clock with DS3231 (Shrike Lite - RP2040)

Hardware real-time clock using the DS3231 module.
Reads the current time from the RTC and displays it on a TM1637 4-digit display.
The colon blinks every second.

Requires tm1637.py and ds3231.py on the board:
  mpremote cp tm1637.py :tm1637.py
  mpremote cp ds3231.py :ds3231.py
  mpremote cp main.py :main.py

Wiring:
  DS3231 SDA -> RP_IO6
  DS3231 SCL -> RP_IO7
  
  TM1637 CLK -> RP_IO14
  TM1637 DIO -> RP_IO15
  
  VCC -> 3.3V, GND -> GND (for both modules)
"""

from machine import Pin, SoftI2C
import tm1637
from ds3231 import DS3231
import time

# Initialize DS3231 RTC
# Using SoftI2C for flexibility
i2c = SoftI2C(scl=Pin(7), sda=Pin(6), freq=100000)
rtc = DS3231(i2c)

# Initialize TM1637 Display
display = tm1637.TM1637(clk=Pin(14), dio=Pin(15))
display.brightness(7)

# UNCOMMENT the lines below ONCE to set the initial time on the RTC module
# Format: (Year, Month, Day, Weekday(0-6), Hour(24H), Minute, Second)
# rtc.datetime((2023, 10, 25, 2, 14, 30, 0))

colon_state = True

while True:
    # Read time from RTC
    YY, MM, DD, wday, hh, mm, ss = rtc.datetime()
    
    # Toggle colon every second
    colon_state = (ss % 2 == 0)
    
    # Update Display
    display.numbers(hh, mm, colon=colon_state)
    
    time.sleep_ms(500)
