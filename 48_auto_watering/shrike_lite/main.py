"""
Project 48: Automated Plant Watering (Shrike Lite - RP2040)

Reads an analog soil moisture sensor and turns on a relay
(which powers a small water pump) when the soil is dry.

Wiring:
  Soil Sensor Analog Out -> RP_IO26 (ADC0)
  Relay IN               -> RP_IO14
  
  Soil Sensor VCC -> 3.3V
  Relay VCC       -> 5V (often required for the relay coil)
  Pump VCC        -> Relay COM/NO and External Power
  All GNDs        -> GND
"""

from machine import Pin, ADC
import time

# Initialize ADC for Soil Sensor (16-bit mapped on RP2040: 0-65535)
soil_sensor = ADC(Pin(26))

# Initialize Relay Pin
relay = Pin(14, Pin.OUT)
relay.value(0) # Ensure pump is off on boot

# Thresholds
# Dry soil = high resistance = high voltage reading (close to 65535)
# Wet soil = low resistance = low voltage reading
DRY_THRESHOLD = 45000

print("Auto Watering System Active.")

while True:
    moisture = soil_sensor.read_u16()
    
    # Calculate percentage (optional, assuming 25000 is completely wet in water, 65535 is completely dry air)
    # Just for better terminal output
    wetness_percent = 100 - ((moisture - 25000) / (65535 - 25000) * 100)
    wetness_percent = max(0, min(100, wetness_percent)) # Clamp 0-100
    
    print(f"Moisture reading: {moisture} ({wetness_percent:.1f}% wet)")
    
    if moisture > DRY_THRESHOLD:
        print("Soil is dry! Watering...")
        relay.value(1) # Turn on pump
    else:
        relay.value(0) # Turn off pump
        
    time.sleep(2)
