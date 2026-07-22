# Project 50: Royal Tetris

A fully playable Tetris clone in MicroPython using just a standard SSD1306 OLED Display and a single Rotary Encoder!

## How it Works
Tetris usually requires 4 buttons (Left, Right, Rotate, Drop). Since a rotary encoder only provides 3 inputs, the controls have been adapted:
- **Turn Knob Left:** Move piece left
- **Turn Knob Right:** Move piece right
- **Press Knob:** Rotate piece 90 degrees
- **Gravity:** The piece falls automatically! The speed increases as you clear lines and level up.

## Hardware Required
- 1x Shrike Fi (ESP32-S3) OR Shrike Lite (RP2040)
- 1x 0.96" I2C/SPI OLED Display (SSD1306)
- 1x Rotary Encoder (KY-040 or raw EC11)
- Jumper wires

## Wiring / Pinout

### Shrike Fi (ESP32-S3)
| Signal | Pin |
|--------|-----|
| OLED SCL (CLK) | `ESP_IO5` |
| OLED SDA (MOSI) | `ESP_IO6` |
| OLED CS | `ESP_IO7` |
| OLED DC | `ESP_IO4` |
| OLED RES | `ESP_IO3` |
| Encoder CLK | `ESP_IO15` |
| Encoder DT | `ESP_IO16` |
| Encoder SW (push) | `ESP_IO17` |

### Shrike Lite (RP2040)
| Signal | Pin |
|--------|-----|
| OLED SCL (CLK) | `RP_IO10` |
| OLED SDA (MOSI) | `RP_IO11` |
| OLED CS | `RP_IO14` |
| OLED DC | `RP_IO15` |
| OLED RES | `RP_IO9` |
| Encoder CLK | `RP_IO16` |
| Encoder DT | `RP_IO17` |
| Encoder SW (push) | `RP_IO26` |

**Encoder Power & Wiring:**
- **If using a KY-040 Module:** Connect `VCC` to `3.3V` and `GND` to `GND`.
- **If using a Raw Encoder:** Wire both the "Common" pin and the "Switch GND" pin directly to `GND`. No VCC required.

## Installation
1. Navigate into the `shrike_lite` or `shrike_fi` folder.
2. Upload both `ssd1306.py` and `main.py` to the board:
   ```bash
   python -m mpremote cp ssd1306.py :ssd1306.py
   python -m mpremote cp main.py :main.py
   ```
3. Reboot to play:
   ```bash
   python -m mpremote reset
   ```
