# 🔢 Button Press Counter

A simple button counter displayed on a **TM1637 4-digit seven-segment display**. Short press to increment, hold for 1.5 seconds to reset to zero. Wraps at 9999.

## Board Variants

| Variant | Board | MCU | Sketch |
|---------|-------|-----|--------|
| [shrike_fi](shrike_fi/) | Shrike Fi | ESP32-S3 | `shrike_fi/button_counter.ino` |
| [shrike_lite](shrike_lite/) | Shrike Lite | RP2040 | `shrike_lite/button_counter.ino` |

## Features

- Short press increments the count
- Hold for 1.5s to reset (display blinks "0000" to confirm)
- Wraps from 9999 back to 0
- Debounced input
- Serial logging of count and resets

## Wiring

### Shrike Fi (ESP32-S3)

| Function | Signal | GPIO |
|----------|--------|------|
| TM1637 CLK | ESP_IO1 | 1 |
| TM1637 DIO | ESP_IO2 | 2 |
| Button | ESP_IO3 | 3 (active low, internal pullup) |

### Shrike Lite (RP2040)

| Function | Signal | GPIO |
|----------|--------|------|
| TM1637 CLK | RP_IO10 | 10 |
| TM1637 DIO | RP_IO11 | 11 |
| Button | RP_IO28 | 28 (active low, internal pullup) |

## Dependencies

Install these via the Arduino Library Manager:

| Library | Author |
|---------|--------|
| **TM1637Display** | Avishay Orpaz |

## Arduino IDE Setup

### Shrike Fi
1. **Board**: `ESP32S3 Dev Module`
2. Open `shrike_fi/button_counter.ino` and upload

### Shrike Lite
1. **Board**: `Raspberry Pi Pico` (arduino-pico core)
2. Open `shrike_lite/button_counter.ino` and upload
