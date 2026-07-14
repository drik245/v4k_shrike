"""
Project 48: Automated Plant Watering (Shrike Fi - ESP32-S3)

Reads an analog soil moisture sensor and turns on a relay
(which powers a small water pump) when the soil is dry.

Wiring:
  Soil Sensor Analog Out -> ESP_IO1 (ADC_CH0)
  Relay IN               -> ESP_IO14
  
  Soil Sensor VCC -> 3.3V
  Relay VCC       -> 5V (often required for the relay coil)
  Pump VCC        -> Relay COM/NO and External Power
  All GNDs        -> GND
"""

from machine import Pin, ADC
import time

# Initialize ADC for Soil Sensor (12-bit: 0-4095)
soil_sensor = ADC(Pin(1), atten=ADC.ATTN_11DB)

# Initialize Relay Pin
relay = Pin(14, Pin.OUT)
relay.value(0) # Ensure pump is off on boot

# Thresholds
# Dry soil = high resistance = high voltage reading (close to 4095)
# Wet soil = low resistance = low voltage reading
DRY_THRESHOLD = 3000

print("Auto Watering System Active.")

while True:
    moisture = soil_sensor.read()
    
    # Calculate percentage (optional, assuming 1500 is completely wet in water, 4095 is completely dry air)
    # Just for better terminal output
    wetness_percent = 100 - ((moisture - 1500) / (4095 - 1500) * 100)
    wetness_percent = max(0, min(100, wetness_percent)) # Clamp 0-100
    
    print(f"Moisture reading: {moisture} ({wetness_percent:.1f}% wet)")
    
    if moisture > DRY_THRESHOLD:
        print("Soil is dry! Watering...")
        relay.value(1) # Turn on pump
    else:
        relay.value(0) # Turn off pump
        
    time.sleep(2)
