# 🖱️ Joystick Mouse

Turn your Vicharak **Shrike Fi** into a wireless **Bluetooth LE HID mouse** using an analog joystick. Tilt the stick to move the cursor; press the button for a left click.

> **Shrike Fi only** — this project requires Bluetooth, which is not available on the Shrike Lite (RP2040).

## Board Variant

| Variant | Board | MCU | Sketch |
|---------|-------|-----|--------|
| [shrike_fi](shrike_fi/) | Shrike Fi | ESP32-S3 | `shrike_fi/joystick_mouse.ino` |

## Features

- **BLE HID mouse** — appears as "Vicharak Joystick" on the host device
- Quadratic acceleration curve for precise low-speed tracking and fast sweeps
- Configurable dead zone to prevent drift
- Debounced joystick button → **left click** (press & hold supported)
- Auto-calibrating joystick center at boot

## Wiring

### Shrike Fi (ESP32-S3)

| Function | Signal | GPIO |
|----------|--------|------|
| Joystick X | ESP_IO1 | 1 (ADC1_CH0) |
| Joystick Y | ESP_IO2 | 2 (ADC1_CH1) |
| Joystick SW | ESP_IO3 | 3 |

No display required — this project only needs the joystick.

## Dependencies

Just make sure you have the **ESP32 board package** installed in Arduino IDE (*Tools → Board → Boards Manager → "esp32"*).

## Arduino IDE Setup

1. **Board**: `ESP32S3 Dev Module` (ESP32 board package)
2. Enable **USB CDC On Boot** if you want Serial Monitor output
3. Select the correct COM port
4. Open `shrike_fi/joystick_mouse.ino` and upload

## Usage

1. Upload the sketch and power the board
2. Keep the joystick centered during the ~1 second calibration phase
3. On your PC / phone, open Bluetooth settings and pair with **"Vicharak Joystick"**
4. Move the joystick to control the cursor
5. Press the joystick button to left-click

## Tuning

These constants in the sketch can be adjusted:

| Constant | Default | Description |
|----------|---------|-------------|
| `DEAD_ZONE` | 80 | ADC counts ignored around center (reduces drift) |
| `MAX_MOUSE_SPEED` | 15 | Maximum pixel movement per cycle |
| `LOOP_DELAY_MS` | 10 | Main loop delay — lower = more responsive |
| `DEBOUNCE_MS` | 50 | Button debounce window |
| `CALIBRATION_SAMPLES` | 64 | Number of ADC reads during calibration |
