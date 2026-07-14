"""
Project 42: LED Bar Graph (Shrike Lite - RP2040)

Reads an analog value from a potentiometer and maps it to a 10-segment
LED bar graph. The higher the voltage, the more LEDs light up.

Wiring:
  Potentiometer Wiper -> RP_IO26 (ADC0)
  Potentiometer VCC   -> 3.3V
  Potentiometer GND   -> GND

  LED 1 (Lowest)  -> RP_IO16 (through 330 ohm resistor to GND)
  LED 2           -> RP_IO17
  LED 3           -> RP_IO18
  LED 4           -> RP_IO19
  LED 5           -> RP_IO20
  LED 6           -> RP_IO21
  LED 7           -> RP_IO22
  LED 8           -> RP_IO23
  LED 9           -> RP_IO24
  LED 10 (Highest)-> RP_IO25
"""

from machine import Pin, ADC
import time

# Initialize potentiometer ADC (16-bit resolution mapped internally in MicroPython RP2040: 0-65535)
pot = ADC(Pin(26))

# Initialize the 10 LED pins
led_pins = [16, 17, 18, 19, 20, 21, 22, 23, 24, 25]
leds = [Pin(p, Pin.OUT) for p in led_pins]

# Ensure all LEDs are off initially
for led in leds:
    led.value(0)

print("LED Bar Graph Active!")

while True:
    # Read ADC value (0-65535 on RP2040)
    val = pot.read_u16()
    
    # Map the 0-65535 value to 0-10 LEDs
    num_leds = int((val / 65536.0) * 11)
    
    # Clamp to max 10
    if num_leds > 10:
        num_leds = 10
        
    # Update LEDs
    for i, led in enumerate(leds):
        if i < num_leds:
            led.value(1)
        else:
            led.value(0)
            
    time.sleep_ms(50)
