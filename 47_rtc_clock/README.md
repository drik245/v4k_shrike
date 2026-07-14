# Project 47: RTC Clock with DS3231

Build a reliable digital clock that keeps time even when powered off! Uses a DS3231 Real-Time Clock (RTC) module with a coin cell battery backup, and a TM1637 4-digit display to show the time. Written in **MicroPython**.

Supports **Shrike Lite** (Note: For Shrike Fi, see Project 16 which uses the ESP32-S3's built-in RTC).

## Hardware Required
| Component | Qty |
|-----------|-----|
| Shrike Lite | 1 |
| DS3231 RTC Module (I2C) | 1 |
| TM1637 4-digit display | 1 |
| CR2032 Coin Cell Battery (for the DS3231) | 1 |

## Wiring / Pinout

### Shrike Lite (RP2040)
| Component | Pin |
|-----------|-----|
| DS3231 SDA | `RP_IO6` |
| DS3231 SCL | `RP_IO7` |
| TM1637 CLK | `RP_IO14` |
| TM1637 DIO | `RP_IO15` |

**Power connections:** 
- Connect the **VCC** of both the DS3231 and TM1637 to **3.3V** or 5V on the Shrike board.
- Connect the **GND** of both modules to **GND** on the Shrike board.

## Software Setup (MicroPython)

The `shrike_lite` folder contains three files: `main.py`, `ds3231.py` (the RTC driver), and `tm1637.py` (the display driver).

1. Flash your board with MicroPython firmware.
2. Navigate into the `shrike_lite` folder.
3. Upload all three files:
   ```bash
   mpremote cp tm1637.py :tm1637.py
   mpremote cp ds3231.py :ds3231.py
   mpremote cp main.py :main.py
   ```
4. **Setting the Time:**
   The first time you run this, you need to set the time on the RTC module.
   - Open `main.py` in your editor.
   - Uncomment the line `rtc.datetime(...)` and enter the current date and time.
   - Save and run the script once. The RTC now has the correct time.
   - Comment the line back out, save, and upload it again so it doesn't overwrite the time on the next reboot.
5. Reboot to run:
   ```bash
   mpremote soft-reset
   ```
