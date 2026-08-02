# 🕐 NTP Digital Clock

WiFi-connected digital clock, synced to NTP time servers. 
Includes two display options:
1. **128×64 SSD1306 OLED** (SPI) - Displays day, date, large time, and seconds progress bar.
2. **TM1637 4-digit seven-segment display** - Displays time with blinking colon.

## Board Variants

| Variant | Board | MCU | Sketch |
|---------|-------|-----|--------|
| [shrike_fi (OLED)](shrike_fi/) | Shrike Fi | ESP32-S3 | `shrike_fi/ntp_clock.ino` |
| [shrike_fi (TM1637)](shrike_fi/) | Shrike Fi | ESP32-S3 | `shrike_fi/ntp_clock_tm1637.ino` |

> Shrike Lite is not supported (no WiFi on RP2040).

## Features

- NTP time sync on boot via `pool.ntp.org`
- **Offline Fallback:** If WiFi fails, seamlessly falls back to an internal oscillator (starting at 12:00) using `millis()`.
- **Auto Reconnect:** Periodically checks WiFi status and automatically resyncs internal clock with NTP when connection returns.
- IST timezone offset pre-configured (configurable)
- **OLED Version:** Day of week, date, large HH:MM:SS, seconds progress bar
- **TM1637 Version:** Simple HH:MM with blinking colon

## Wiring

### Shrike Fi (ESP32-S3) - OLED Version

| Function | Signal | GPIO |
|----------|--------|------|
| OLED MOSI | ESP_IO35 | 35 |
| OLED CLK | ESP_IO36 | 36 |
| OLED DC | ESP_IO37 | 37 |
| OLED RST | ESP_IO38 | 38 |
| OLED CS | ESP_IO34 | 34 |
| OLED VCC | 3.3V | — |
| OLED GND | GND | — |

### Shrike Fi (ESP32-S3) - TM1637 Version

| Function | Signal | GPIO |
|----------|--------|------|
| TM1637 CLK | ESP_IO1 | 1 |
| TM1637 DIO | ESP_IO2 | 2 |

## Dependencies

Install these via the Arduino Library Manager:

| Library | Author | For Sketch |
|---------|--------|------------|
| **Adafruit SSD1306** | Adafruit | `ntp_clock.ino` |
| **Adafruit GFX** | Adafruit | `ntp_clock.ino` |
| **TM1637Display** | Avishay Orpaz | `ntp_clock_tm1637.ino` |

## Arduino IDE Setup

1. **Board**: `ESP32S3 Dev Module` (ESP32 board package)
2. Edit `ssid` and `password` at the top of the sketch
3. Adjust `gmtOffset_sec` if you're not in IST (default: 19800 for +5:30)
4. Upload to your Shrike Fi

## Usage

1. Power on — the OLED shows "Connecting WiFi..." then "Syncing NTP..."
2. Once synced, the clock runs continuously and updates every second
