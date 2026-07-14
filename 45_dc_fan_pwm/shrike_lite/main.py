"""
Project 45: DC Fan Speed Control (Shrike Lite - RP2040)

Control the speed of a DC Fan using a Potentiometer.
The RP2040 reads the analog voltage from the potentiometer and generates
a PWM signal to drive a MOSFET (or L298N/Relay module) connected to the fan.

Wiring:
  Potentiometer Wiper -> RP_IO26 (ADC0)
  Potentiometer VCC   -> 3.3V
  Potentiometer GND   -> GND

  MOSFET Gate (PWM)   -> RP_IO14
  MOSFET Source       -> GND
  MOSFET Drain        -> Fan Negative (-)
  Fan Positive (+)    -> External Power (e.g., 5V or 12V depending on fan)
"""

from machine import Pin, ADC, PWM
import time

# Initialize ADC for potentiometer
pot = ADC(Pin(26))

# Initialize PWM for the fan (MOSFET)
fan = PWM(Pin(14), freq=1000)

print("DC Fan PWM Control Active.")

while True:
    # Read ADC value (0-65535 on RP2040)
    val = pot.read_u16()
    
    # The read_u16() method already returns 0-65535,
    # which maps perfectly to the 16-bit duty_u16() requirement.
    fan.duty_u16(val)
    
    time.sleep_ms(20)
