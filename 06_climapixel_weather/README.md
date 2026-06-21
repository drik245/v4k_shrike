# 🌤️ ClimaPixel Weather Display

A mini weather station that reads temperature and humidity data from a **DHT22 sensor** and visualizes it on a **128×64 SSD1306 OLED** display. 

The UI features custom-coded binary bitmap icons for the thermometer and water droplet, matching the styling of the original MicroPython ClimaPixel project but written in C++ for the ESP32-S3.

## Board Variants

| Variant | Board | MCU | Sketch |
|---------|-------|-----|--------|
| [shrike_fi](shrike_fi/) | Shrike Fi | ESP32-S3 | `shrike_fi/climapixel_weather.ino` |

> Shrike Lite is not supported out-of-the-box in this sketch, though it can easily be adapted.

## Features

- Local temperature and humidity monitoring using a DHT22 sensor
- No internet/WiFi connection required
- Custom bitmap graphics for UI icons
- Smooth display updates and error handling if the sensor is disconnected
- Clean layout with separate rows for Temp and Hum

## Wiring

### Shrike Fi (ESP32-S3)

| Function | Signal | GPIO |
|----------|--------|------|
| DHT22 Data | ESP_IO4 | 4 |
| OLED MOSI | ESP_IO35 | 35 |
| OLED CLK | ESP_IO36 | 36 |
| OLED DC | ESP_IO37 | 37 |
| OLED RST | ESP_IO38 | 38 |
| OLED CS | ESP_IO34 | 34 |
| DHT22 & OLED VCC | 3.3V | — |
| DHT22 & OLED GND | GND | — |

> A 10K pull-up resistor between the DHT22 data pin and VCC is recommended for reliable readings.

## Dependencies

Install these via the Arduino Library Manager:

| Library | Author |
|---------|--------|
| **DHT sensor library for ESPx** | beegee-tokyo |
| **Adafruit SSD1306** | Adafruit |
| **Adafruit GFX** | Adafruit |

## Arduino IDE Setup

1. **Board**: `ESP32S3 Dev Module` (ESP32 board package)
2. Select the correct COM port
3. Open `shrike_fi/climapixel_weather.ino` and upload

## Usage

1. Wire up the OLED and DHT22 as per the table above.
2. Flash the code to the Shrike Fi.
3. The display will show a splash screen for 2 seconds, then begin showing live temperature and humidity updates every 2 seconds.
4. If the sensor is disconnected, the display will show a "Sensor Error" message.
