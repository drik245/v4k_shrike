# 🎛️ Servo Ctrl with OLED Gauge

Joystick-controlled servo with a **real-time needle gauge** on a 128×64 SSD1306 OLED. Tilt the X-axis to sweep the servo from 0° to 180° — the OLED displays a half-circle gauge that tracks the angle live, complete with arc ticks, a rotating needle, and a large degree readout.

## Board Variants

| Variant | Board | MCU | Sketch |
|---------|-------|-----|--------|
| [shrike_fi](shrike_fi/) | Shrike Fi | ESP32-S3 | `shrike_fi/servo_ctrl.ino` |
| [shrike_lite](shrike_lite/) | Shrike Lite | RP2040 | `shrike_lite/servo_ctrl.ino` |

## Features

- **0°–180° servo sweep** controlled by joystick X-axis
- **OLED needle gauge** — half-circle arc with tick marks at 0°, 45°, 90°, 135°, 180°
- Large **angle readout** with degree symbol
- Dead-zone filtering — servo snaps to 90° when the joystick is centered
- Auto-calibrating joystick center at boot
- Serial debug output every 500 ms

## Wiring

### Shrike Fi (ESP32-S3)

| Function | Signal | GPIO |
|----------|--------|------|
| Joystick X | ESP_IO1 | 1 (ADC1_CH0) |
| Joystick Y | ESP_IO2 | 2 (ADC1_CH1) |
| Joystick SW | ESP_IO3 | 3 |
| OLED MOSI | ESP_IO35 | 35 |
| OLED CLK | ESP_IO36 | 36 |
| OLED DC | ESP_IO37 | 37 |
| OLED RST | ESP_IO38 | 38 |
| OLED CS | ESP_IO34 | 34 |
| **Servo signal** | **ESP_IO4** | **4** |

### Shrike Lite (RP2040)

| Function | Signal | GPIO |
|----------|--------|------|
| Joystick X | RP_IO26 | 26 (ADC0) |
| Joystick Y | RP_IO27 | 27 (ADC1) |
| Joystick SW | RP_IO28 | 28 |
| OLED MOSI | RP_IO7 | 7 (SPI0 TX) |
| OLED CLK | RP_IO6 | 6 (SPI0 SCK) |
| OLED DC | RP_IO8 | 8 |
| OLED RST | RP_IO9 | 9 |
| OLED CS | RP_IO5 | 5 (SPI0 CSn) |
| **Servo signal** | **RP_IO10** | **10** |

> **Note:** The servo signal wire goes to the servo's signal pin. The servo also needs **5 V** and **GND** — use the board's 5 V rail (or an external supply for high-torque servos).

## Dependencies

Install these via the Arduino Library Manager:

| Library | Used by |
|---------|---------|
| **Adafruit SSD1306** | Both |
| **Adafruit GFX** | Both |
| **ESP32Servo** | Shrike Fi only |
| **Servo** (built-in) | Shrike Lite only |

## Arduino IDE Setup

### Shrike Fi
1. **Board**: `ESP32S3 Dev Module` (ESP32 board package)
2. Select the correct COM port
3. Open `shrike_fi/servo_ctrl.ino` and upload

### Shrike Lite
1. **Board**: `Raspberry Pi Pico` (arduino-pico core by Earle Philhower)
2. Select the correct COM port
3. Open `shrike_lite/servo_ctrl.ino` and upload

## Usage

1. Connect the servo and OLED as shown in the wiring tables above
2. Upload the appropriate sketch for your board
3. Keep the joystick centered during the ~1 second calibration splash screen
4. Tilt the joystick **left** → servo moves toward 0°
5. Tilt the joystick **right** → servo moves toward 180°
6. Release to center → servo returns to 90°
7. The OLED gauge needle and angle readout update in real time
