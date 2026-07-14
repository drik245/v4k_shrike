# Project 38: Rotary Encoder Menu System

A scrollable OLED UI menu driven by a rotary encoder knob. Turn to navigate, click to select. Written in **MicroPython**, works on both Shrike Lite and Shrike Fi.

## Features
- Scrollable 3-row menu with a live `>` cursor arrow
- Scroll indicator bar on the right edge of the display
- Press-to-select runs a per-item demo action, then returns to the menu
- Proper encoder edge-detection — no polling jitter
- 50ms button debounce built in

## Hardware Required
| Component | Qty |
|-----------|-----|
| Shrike Lite or Shrike Fi | 1 |
| SSD1306 OLED (128×64, **SPI**) | 1 |
| KY-040 Rotary Encoder Module | 1 |

## Wiring / Pinout

### Shrike Lite (RP2040)
| Signal | Pin |
|--------|-----|
| OLED SCL (CLK) | `RP_IO6` (SPI0 SCK) |
| OLED SDA (MOSI) | `RP_IO7` (SPI0 TX) |
| OLED CS | `RP_IO5` (SPI0 CSn) |
| OLED DC | `RP_IO8` |
| OLED RES | `RP_IO9` |
| Encoder CLK | `RP_IO14` |
| Encoder DT | `RP_IO15` |
| Encoder SW (push) | `RP_IO10` |

### Shrike Fi (ESP32-S3)
| Signal | Pin |
|--------|-----|
| OLED SCL (CLK) | `ESP_IO36` (SPI2_CLK) |
| OLED SDA (MOSI) | `ESP_IO35` (SPI2_MOSI) |
| OLED CS | `ESP_IO34` (SPI2_CS0) |
| OLED DC | `ESP_IO4` |
| OLED RES | `ESP_IO5` |
| Encoder CLK | `ESP_IO1` |
| Encoder DT | `ESP_IO2` |
| Encoder SW (push) | `ESP_IO3` |

OLED and Encoder VCC → 3.3V, GND → GND.

## Software Setup (MicroPython)

1. Flash your board with MicroPython firmware.
2. The `ssd1306` driver is included in the standard MicroPython firmware — no extra install needed.
3. Navigate into the `shrike_lite` or `shrike_fi` folder.
4. Upload `main.py` to the board:
   ```bash
   mpremote cp main.py :main.py
   ```
5. Reboot to run:
   ```bash
   mpremote soft-reset
   ```

## How to Use
- **Turn knob left/right** to move the `>` cursor up and down.
- **Press the knob** to select the highlighted item.
- Each item runs a short demo and returns to the menu after 2 seconds.

## Menu Items
| Item | What it does |
|------|-------------|
| LED Blink | Blinks the onboard LED 5 times (Shrike Lite only) |
| Show Info | Displays the board name and firmware on OLED |
| Counter | Counts down from 5 to 0 on the OLED |
| About | Shows project info |
