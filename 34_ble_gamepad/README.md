# Project 34: BLE Xbox-Style Gamepad (Shrike Fi)

Turn your Shrike Fi into a **wireless Bluetooth gamepad** that shows up on your PC exactly like an Xbox controller — complete with **dual analog thumbsticks** and **4 face buttons (A, B, X, Y)**.

> [!WARNING]
> **Disclaimer:** This project serves as a proof of concept. The standard KY-023 Arduino joystick modules have poor mechanical return-to-center and large deadzones, making them unsuitable for serious or competitive gaming. Furthermore, because this exposes a standard generic HID device rather than a proprietary XInput device, many modern PC games will not recognize it natively without the use of translation software like Steam Input or x360ce. It is, however, a great educational project for learning BLE HID and analog mapping!

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
| SW  | `ESP_IO2` (L3 Button) |

### Right Thumbstick
| Joystick Pin | Shrike Fi Pin |
|---|---|
| VCC | 3.3V |
| GND | GND |
| VRX | `ESP_IO6` |
| VRY | `ESP_IO7` |
| SW  | `ESP_IO3` (R3 Button) |

> **⚠️ CRITICAL:** You **MUST** connect the Joystick VCC to `3.3V`, NOT `5V`! The ESP32-S3 ADC pins cannot handle 5V. If you use 5V, you will experience severe "crosstalk" (moving one stick will cause the other stick to glitch and move on-screen).

### Face Buttons (wire to GND when pressed, no external resistor needed)
| Button | Shrike Fi Pin | Maps to |
|--------|---------------|----------|
| A | `ESP_IO14` | Button 1 |
| B | `ESP_IO15` | Button 2 |
| X | `ESP_IO16` | Button 3 |
| Y | `ESP_IO17` | Button 4 |

## Software Setup
1. In Arduino IDE, open **Sketch → Include Library → Manage Libraries**.
2. Search for and install **NimBLE-Arduino** (by h2zero). *This is required for the gamepad library to compile!*
3. Search for and install **ESP32 BLE Gamepad** (by lemmingDev).
4. Navigate into the `shrike_fi` folder and open `ble_gamepad.ino`.
5. Select board: `ESP32S3 Dev Module`.
6. Compile and upload.

### Auto-Calibration
The controller features an on-device, dynamic auto-calibration system that completely eliminates drift and maximizes range automatically.

1. **When powering on the Shrike Fi, DO NOT touch the joysticks.** The firmware takes 20 rapid samples at boot to establish a perfect, drift-free center point.
2. After it boots, simply **rotate both sticks in a few wide, full circles**. The firmware will automatically detect the physical limits of your joysticks and stretch the output range to a perfect 0-100%.

### Troubleshooting: Reversed or Swapped Directions?
Depending on how you physically mounted the joystick modules, they might move in the wrong direction.
- **Reversed?** (e.g., pushing UP makes the character look DOWN)
- **Sideways?** (e.g., pushing UP makes the character look LEFT, because the module is rotated 90 degrees)

To fix this, simply open `ble_gamepad.ino` and flip the `true` or `false` flags at the very top of the file:
```cpp
// --- CONFIGURATION ---
// Change these to 'true' if any axis moves in the wrong direction!
#define INVERT_LEFT_X  true
#define INVERT_LEFT_Y  true
#define INVERT_RIGHT_X false
#define INVERT_RIGHT_Y false

// Change these to 'true' if up/down is swapped with left/right (mounted sideways)
#define SWAP_LEFT_AXES false
#define SWAP_RIGHT_AXES false
```
Re-upload the sketch, and your directions will be instantly fixed!

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

## Future Improvements
To make this a fully-featured controller, you can easily expand this project later by adding:
- **D-Pad (Up/Down/Left/Right):** Map 4 extra buttons to `bleGamepad.setHat()`.
- **Bumpers & Menu (L1/R1, Start, Select):** The ESP32-S3 has plenty of free pins to wire up extra `INPUT_PULLUP` buttons and map them to `BUTTON_5` through `BUTTON_8`.
- **L2/R2 Analog Triggers:** Add two potentiometers and map them to the `Z` and `rZ` axes.
- **OLED Status Display:** Wire an I2C OLED to show Bluetooth connection status, battery level, or current button inputs.
- **Vibration Feedback:** Wire a small coin vibration motor to a PWM pin and trigger it via the game's rumble output!
- **IoT Remote Control:** While not precise enough for gaming, this controller is absolutely perfect for driving DIY IoT robots, RC cars, drones, or controlling smart home appliances!
