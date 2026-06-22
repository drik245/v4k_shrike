# 🌦️ Mini Weather Station

A localized mini weather station that reads temperature and humidity from a **DHT22 sensor** and displays it on a classic **16x2 Character LCD** running in 4-bit parallel mode (no I2C required).

## Board Variants

| Variant | Board | MCU | Sketch |
|---------|-------|-----|--------|
| [shrike_fi](shrike_fi/) | Shrike Fi | ESP32-S3 | `shrike_fi/mini_weather_station.ino` |

## Features

- Real-time temperature (°C) and humidity (%) display
- Uses standard 4-bit parallel LCD communication (LiquidCrystal library)
- Automatic LCD text padding to prevent display artifacts
- Error handling if the DHT22 sensor gets disconnected

## Wiring

### Shrike Fi (ESP32-S3)

Since this uses a raw 16x2 LCD without an I2C backpack, it requires several data lines.

| LCD Pin | Function | Shrike Fi GPIO |
|---------|----------|----------------|
| 1 | VSS (GND) | GND |
| 2 | VDD (5V) | 5V / VBUS |
| 3 | V0 (Contrast) | 10k Potentiometer wiper (or 1k resistor to GND) |
| 4 | RS (Register Select) | ESP_IO2 (2) |
| 5 | R/W (Read/Write) | GND (Write only) |
| 6 | E (Enable) | ESP_IO1 (1) |
| 11 | D4 | ESP_IO3 (3) |
| 12 | D5 | ESP_IO4 (4) |
| 13 | D6 | ESP_IO5 (5) |
| 14 | D7 | ESP_IO6 (6) |
| 15 | A (Backlight Anode) | 5V / VBUS (via 220Ω resistor) |
| 16 | K (Backlight Cathode)| GND |

| Sensor Pin | Function | Shrike Fi GPIO |
|------------|----------|----------------|
| DHT22 Data | Signal | ESP_IO7 (7) |
| DHT22 VCC | Power | 3.3V or 5V |
| DHT22 GND | Ground | GND |

> **Note on LCD Contrast:** If your screen is blank or showing solid square boxes, adjust the contrast potentiometer on Pin 3 until the text becomes clear. The LCD logic runs perfectly on the ESP32's 3.3V logic signals, but the LCD itself must be powered with 5V on VDD (Pin 2).

## Dependencies

Install these via the Arduino Library Manager:

| Library | Author |
|---------|--------|
| **LiquidCrystal** | Arduino / Adafruit (built-in) |
| **DHT sensor library for ESPx** | beegee-tokyo |

## Arduino IDE Setup

1. **Board**: `ESP32S3 Dev Module` (ESP32 board package)
2. Select the correct COM port
3. Open `shrike_fi/mini_weather_station.ino` and upload
