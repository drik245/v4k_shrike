# 💡 Touch LED Control

Toggles the onboard LED using a touch sensor. Supports single tap (toggle on/off) and double tap (start/stop blinking).

## Board Variants

| Variant | Board | MCU | Sketch |
|---------|-------|-----|--------|
| [shrike_fi](shrike_fi/) | Shrike Fi | ESP32-S3 | `shrike_fi/touch_led_control.ino` |
| [shrike_lite](shrike_lite/) | Shrike Lite | RP2040 | `shrike_lite/touch_led_control.ino` |

## Features

- **Single tap** — toggle LED on/off
- **Double tap** — start blinking mode
- **Double tap again** — stop blinking
- Debounced input with 300ms double-tap detection window
- Non-blocking blink using `millis()`

## Wiring

### Shrike Fi (ESP32-S3)

Uses the ESP32-S3's built-in capacitive touch peripheral.

| Function | Signal | GPIO |
|----------|--------|------|
| Touch input | ESP_IO1 | 1 (Touch IO 1) |
| Onboard LED | ESP_IO21 | 21 |

### Shrike Lite (RP2040)

Uses an external digital touch sensor (TTP223 or similar).

| Function | Signal | GPIO |
|----------|--------|------|
| Touch sensor | RP_IO27 | 27 |
| Onboard LED | RP_IO4 | 4 |

## Dependencies

No external libraries needed.

## Arduino IDE Setup

### Shrike Fi
1. **Board**: `ESP32S3 Dev Module`
2. Open `shrike_fi/touch_led_control.ino` and upload

### Shrike Lite
1. **Board**: `Raspberry Pi Pico` (arduino-pico core)
2. Open `shrike_lite/touch_led_control.ino` and upload
