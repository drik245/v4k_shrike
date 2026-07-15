# Project 41: Automatic Telegrapher Keyer

Tap morse code on a button and watch it decode in real time on the OLED display. The screen shows your current dots/dashes as you enter them, and automatically decodes each letter after a short pause. Written in **MicroPython**.

Supports both **Shrike Lite** and **Shrike Fi**.

## Hardware Required
| Component | Qty |
|-----------|-----|
| Shrike Lite or Shrike Fi | 1 |
| SSD1306 OLED (128×64, SPI) | 1 |
| Momentary push buttons | 3 |
| Active or Passive Buzzer | 1 |

## Wiring / Pinout

### Shrike Lite (RP2040)
| Signal | Pin |
|--------|-----|
| OLED SCL (CLK) | `RP_IO6` (SPI0 SCK) |
| OLED SDA (MOSI) | `RP_IO7` (SPI0 TX) |
| OLED CS | `RP_IO5` |
| OLED DC | `RP_IO8` |
| OLED RES | `RP_IO9` |
| **Dot Button** | `RP_IO11` → GND |
| **Dash Button** | `RP_IO14` → GND |
| **Clear Button** | `RP_IO15` → GND |
| **Buzzer** | `RP_IO10` |

### Shrike Fi (ESP32-S3)
| Signal | Pin |
|--------|-----|
| OLED SCL (CLK) | `ESP_IO36` (SPI2_CLK) |
| OLED SDA (MOSI) | `ESP_IO35` (SPI2_MOSI) |
| OLED CS | `ESP_IO34` |
| OLED DC | `ESP_IO4` |
| OLED RES | `ESP_IO5` |
| **Dot Button** | `ESP_IO11` → GND |
| **Dash Button** | `ESP_IO14` → GND |
| **Clear Button** | `ESP_IO15` → GND |
| **Buzzer** | `ESP_IO10` |

OLED VCC → 3.3V, GND → GND. Buttons use `INPUT_PULLUP`, no resistors needed. Connect the buzzer's positive pin to the IO pin and negative to GND.

## Software Setup (MicroPython)

1. Flash your board with MicroPython firmware.
2. Navigate to `41_morse_decoder/driver` and upload the OLED driver:
   ```bash
   cd driver
   mpremote cp ssd1306.py :ssd1306.py
   cd ..
   ```
3. Navigate into `shrike_lite` or `shrike_fi`.
4. Upload `main.py` and run it:
   ```bash
   mpremote cp main.py :main.py
   mpremote soft-reset
   ```

## How It Works

Instead of trying to time your button presses perfectly, this updated version uses **three separate buttons**:
1. **Dot Button**: Instantly types a dot (`.`) and plays a short 100ms beep.
2. **Dash Button**: Instantly types a dash (`-`) and plays a longer 300ms beep.
3. **Clear Button**: Wipes the screen and resets the current input.

**Auto-Repeat Feature:** If you hold down the Dot or Dash button, it will continuously type that symbol at a fixed rate, exactly like an automatic telegraph keyer!

You can adjust the speed of the beeps and the repeat rate easily by changing `DOT_BEEP_MS`, `DASH_BEEP_MS`, and `SYMBOL_SPACE_MS` at the top of the `main.py` script!

### Timing Reference
| Action | Duration |
|--------|----------|
| Pause to decode letter | ≥ 600 ms of inactivity |
| Pause for word space | ≥ 1500 ms of inactivity |

### Display Layout
```
Morse Decoder
─────────────
In: .-

HI THERE
```
- **"In:"** line shows dots/dashes as you type the current letter.
- The bottom lines show the full decoded text, scrolling as you go.
- Unknown sequences show as `?`.

## Morse Code Quick Reference
| Letter | Code | | Letter | Code |
|--------|------|-|--------|------|
| A | `.-` | | N | `-.` |
| B | `-...` | | O | `---` |
| C | `-.-.` | | P | `.--.` |
| D | `-..` | | Q | `--.-` |
| E | `.` | | R | `.-.` |
| F | `..-.` | | S | `...` |
| G | `--.` | | T | `-` |
| H | `....` | | U | `..-` |
| I | `..` | | V | `...-` |
| J | `.---` | | W | `.--` |
| K | `-.-` | | X | `-..-` |
| L | `.-..` | | Y | `-.--` |
| M | `--` | | Z | `--..` |
