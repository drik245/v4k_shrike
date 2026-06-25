# 🚨 Smart IR Object Detection System

A simple and efficient object detection system that monitors an area using an **Infrared (IR) Sensor** and triggers both a **visual (LED)** and **auditory (Buzzer)** alert when an object is detected.

## Board Variants

| Variant | Board | MCU | Sketch |
|---------|-------|-----|--------|
| [shrike_fi](shrike_fi/) | Shrike Fi | ESP32-S3 | `shrike_fi/ir_object_detection.ino` |
| [shrike_lite](shrike_lite/) | Shrike Lite | RP2040 | `shrike_lite/ir_object_detection.ino` |

## Features

- Real-time object detection
- Visual LED alert
- Auditory Buzzer alert
- Quick monitoring loop with Serial logging

## Wiring

### Shrike Fi (ESP32-S3)

| Component | Function | Shrike Fi GPIO |
|-----------|----------|----------------|
| IR Sensor OUT | Signal | ESP_IO1 (1) |
| LED | Positive | ESP_IO2 (2) |
| Buzzer | Positive | ESP_IO3 (3) |

*(Connect GND of all components to the board's GND, and power the IR sensor from the 3.3V pin to avoid sending 5V logic to the ESP32).*

### Shrike Lite (RP2040)

| Component | Function | Shrike Lite GPIO |
|-----------|----------|------------------|
| IR Sensor OUT | Signal | RP_IO26 (26) |
| LED | Positive | RP_IO27 (27) |
| Buzzer | Positive | RP_IO28 (28) |

*(Connect GND of all components to the board's GND, and power the IR sensor from the 3.3V pin).*

## Dependencies

No external libraries are required.

## Arduino IDE Setup

### Shrike Fi
1. **Board**: `ESP32S3 Dev Module` (ESP32 board package)
2. Open `shrike_fi/ir_object_detection.ino` and upload

### Shrike Lite
1. **Board**: `Raspberry Pi Pico` (arduino-pico core)
2. Open `shrike_lite/ir_object_detection.ino` and upload
