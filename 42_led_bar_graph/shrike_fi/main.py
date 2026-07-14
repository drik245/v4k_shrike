"""
Project 42: LED Bar Graph (Shrike Fi - ESP32-S3)

Reads an analog value from a potentiometer and maps it to a 10-segment
LED bar graph. The higher the voltage, the more LEDs light up.

Wiring:
  Potentiometer Wiper -> ESP_IO1 (ADC1_CH0)
  Potentiometer VCC   -> 3.3V
  Potentiometer GND   -> GND

  LED 1 (Lowest)  -> ESP_IO15 (through 330 ohm resistor to GND)
  LED 2           -> ESP_IO16
  LED 3           -> ESP_IO17
  LED 4           -> ESP_IO18
  LED 5           -> ESP_IO33
  LED 6           -> ESP_IO34
  LED 7           -> ESP_IO35
  LED 8           -> ESP_IO36
  LED 9           -> ESP_IO37
  LED 10 (Highest)-> ESP_IO38
"""

from machine import Pin, ADC
import time

# Initialize potentiometer ADC (12-bit resolution: 0-4095)
pot = ADC(Pin(1), atten=ADC.ATTN_11DB)

# Initialize the 10 LED pins
led_pins = [15, 16, 17, 18, 33, 34, 35, 36, 37, 38]
leds = [Pin(p, Pin.OUT) for p in led_pins]

# Ensure all LEDs are off initially
for led in leds:
    led.value(0)

print("LED Bar Graph Active!")

while True:
    # Read ADC value (0-4095 on ESP32-S3)
    val = pot.read()
    
    # Map the 0-4095 value to 0-10 LEDs
    # We add a tiny bit to 4095 to ensure we can hit 10 at the very max
    num_leds = int((val / 4096.0) * 11) 
    
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
