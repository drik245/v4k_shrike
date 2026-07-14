"""
Project 45: DC Fan Speed Control (Shrike Fi - ESP32-S3)

Control the speed of a DC Fan using a Potentiometer.
The ESP32 reads the analog voltage from the potentiometer and generates
a PWM signal to drive a MOSFET (or L298N/Relay module) connected to the fan.

Wiring:
  Potentiometer Wiper -> ESP_IO1 (ADC_CH0)
  Potentiometer VCC   -> 3.3V
  Potentiometer GND   -> GND

  MOSFET Gate (PWM)   -> ESP_IO14
  MOSFET Source       -> GND
  MOSFET Drain        -> Fan Negative (-)
  Fan Positive (+)    -> External Power (e.g., 5V or 12V depending on fan)
"""

from machine import Pin, ADC, PWM
import time

# Initialize ADC for potentiometer
pot = ADC(Pin(1), atten=ADC.ATTN_11DB)

# Initialize PWM for the fan (MOSFET)
fan = PWM(Pin(14), freq=1000)

print("DC Fan PWM Control Active.")

while True:
    # Read ADC value (0-4095 on ESP32-S3)
    val = pot.read()
    
    # Map 12-bit ADC (0-4095) to 16-bit PWM (0-65535)
    # val * 16 is a simple way to approximate it (4095 * 16 = 65520)
    duty = val * 16
    
    # Apply the duty cycle to the fan
    fan.duty_u16(duty)
    
    time.sleep_ms(20)
