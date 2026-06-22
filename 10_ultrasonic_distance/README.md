# 📏 Ultrasonic OLED Distance Meter

Measures distance using an **HC-SR04** ultrasonic sensor and displays the reading on a **128×64 SSD1306 OLED** (SPI). Shows a large number readout and a fill bar. Uses 5-sample moving average for smooth readings.

## Board Variants

| Variant | Board | MCU | Sketch |
|---------|-------|-----|--------|
| [shrike_fi](shrike_fi/) | Shrike Fi | ESP32-S3 | `shrike_fi/ultrasonic_distance.ino` |
| [shrike_lite](shrike_lite/) | Shrike Lite | RP2040 | `shrike_lite/ultrasonic_distance.ino` |

## Features

- Large 3x text distance readout in cm
- Bar graph showing distance visually
- 5-sample moving average for stable readings
- "No echo" message when sensor gets no return signal
- Serial logging of all readings

## Wiring

### Shrike Fi (ESP32-S3)

| Function | Signal | GPIO |
|----------|--------|------|
| HC-SR04 Trig | ESP_IO4 | 4 |
| HC-SR04 Echo | ESP_IO5 | 5 |
| OLED MOSI | ESP_IO35 | 35 |
| OLED CLK | ESP_IO36 | 36 |
| OLED DC | ESP_IO37 | 37 |
| OLED RST | ESP_IO38 | 38 |
| OLED CS | ESP_IO34 | 34 |

### Shrike Lite (RP2040)

| Function | Signal | GPIO |
|----------|--------|------|
| HC-SR04 Trig | RP_IO26 | 26 |
| HC-SR04 Echo | RP_IO27 | 27 |
| OLED MOSI | RP_IO7 | 7 |
| OLED CLK | RP_IO6 | 6 |
| OLED DC | RP_IO8 | 8 |
| OLED RST | RP_IO9 | 9 |
| OLED CS | RP_IO5 | 5 |

## Dependencies

| Library | Used by |
|---------|---------|
| **Adafruit SSD1306** | Both |
| **Adafruit GFX** | Both |

## Arduino IDE Setup

### Shrike Fi
1. **Board**: `ESP32S3 Dev Module`
2. Open `shrike_fi/ultrasonic_distance.ino` and upload

### Shrike Lite
1. **Board**: `Raspberry Pi Pico` (arduino-pico core)
2. Open `shrike_lite/ultrasonic_distance.ino` and upload
