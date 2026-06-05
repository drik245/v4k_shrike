# 🐍 Snake Game

Classic snake game running on a **128×64 SSD1306 OLED** (SPI), controlled with an analog joystick. Supports both Vicharak Shrike boards.

## Board Variants

This project ships with two separate sketches — one per board — because the pin mappings, ADC peripheral calls, and high-score storage differ between the RP2040 and ESP32-S3.

| Variant | Board | MCU | Sketch |
|---------|-------|-----|--------|
| [shrike_fi](shrike_fi/) | Shrike Fi | ESP32-S3 | `shrike_fi/snake_game.ino` |
| [shrike_lite](shrike_lite/) | Shrike Lite | RP2040 | `shrike_lite/snake_game.ino` |

## Features

- **4×4-pixel grid** on a 128×64 display (32×16 cells)
- Animated pulsing food orb
- Wall & self-collision detection
- Persistent **high-score** storage (Preferences on ESP32, EEPROM emulation on RP2040)
- **Pause menu** with Resume / Speed / Exit options
- 4 speed presets: Slow · Normal · Fast · Turbo
- Auto-calibrating joystick center at boot

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

## Dependencies

Install these via the Arduino Library Manager:

| Library | Used by |
|---------|---------|
| **Adafruit SSD1306** | Both |
| **Adafruit GFX** | Both |
| **Preferences** (built-in) | Shrike Fi only |
| **EEPROM** (built-in) | Shrike Lite only |

## Arduino IDE Setup

### Shrike Fi
1. **Board**: `ESP32S3 Dev Module` (ESP32 board package)
2. Select the correct COM port
3. Open `shrike_fi/snake_game.ino` and upload

### Shrike Lite
1. **Board**: `Raspberry Pi Pico` (arduino-pico core by Earle Philhower)
2. Select the correct COM port
3. Open `shrike_lite/snake_game.ino` and upload

## Controls

| Input | Action |
|-------|--------|
| Joystick | Steer the snake (up / down / left / right) |
| Button press (in-game) | Open pause menu |
| Joystick up/down (pause) | Navigate menu items |
| Joystick left/right (pause) | Adjust speed setting |
| Button press (pause) | Select menu item |
| Button press (game over) | Restart game |
