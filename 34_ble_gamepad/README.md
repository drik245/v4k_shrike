# Project 34: BLE Xbox-Style Gamepad (Shrike Fi)

Turn your Shrike Fi into a **wireless Bluetooth gamepad** that shows up on your PC exactly like an Xbox controller — complete with **dual analog thumbsticks** and **4 face buttons (A, B, X, Y)**.

> **Board:** Shrike Fi only (ESP32-S3 has built-in Bluetooth)

## Hardware Required
| Component | Qty |
|-----------|-----|
| Shrike Fi | 1 |
| KY-023 Joystick Module (or any analog thumbstick) | 2 |
| Momentary push button | 4 |

## Wiring / Pinout

### Left Thumbstick
| Joystick Pin | Shrike Fi Pin |
|---|---|
| VCC | 3.3V |
| GND | GND |
| VRX | `ESP_IO4` |
| VRY | `ESP_IO5` |

### Right Thumbstick
| Joystick Pin | Shrike Fi Pin |
|---|---|
| VCC | 3.3V |
| GND | GND |
| VRX | `ESP_IO6` |
| VRY | `ESP_IO7` |

### Face Buttons (wire to GND when pressed, no external resistor needed)
| Button | Shrike Fi Pin | Maps to |
|--------|---------------|----------|
| A | `ESP_IO14` | Button 1 |
| B | `ESP_IO15` | Button 2 |
| X | `ESP_IO16` | Button 3 |
| Y | `ESP_IO17` | Button 4 |

## Software Setup
1. In Arduino IDE, open **Sketch → Include Library → Manage Libraries**.
2. Search for **"ESP32 BLE Gamepad"** (by lemmingDev) and install it.
3. Navigate into the `shrike_fi` folder and open `ble_gamepad.ino`.
4. Select board: `ESP32S3 Dev Module`.
5. Compile and upload.

## How to Use
1. After flashing, open your PC's **Bluetooth settings**.
2. Add a new device — you will see **"Shrike Controller"** appear.
3. Pair with it. Your PC will recognize it as a standard HID Gamepad.
4. Open **Control Panel → Devices and Printers → Game Controllers** to calibrate and verify all axes and buttons are working.
5. Load any game, emulator, or tool that supports a standard gamepad and enjoy!

## Features
- **Analog deadzone** applied automatically — no stick drift when centered.
- **~66 Hz polling rate** with batched HID reports for smooth, low-latency input.
- Reports 16 buttons in HID descriptor so it is compatible with the widest range of software.
