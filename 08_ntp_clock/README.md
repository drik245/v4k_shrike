# 🕐 NTP Digital Clock

WiFi-connected digital clock on a **128×64 SSD1306 OLED** (SPI), synced to NTP time servers. Displays the day of the week, date, large HH:MM:SS time, and a seconds progress bar.

## Board Variants

| Variant | Board | MCU | Sketch |
|---------|-------|-----|--------|
| [shrike_fi](shrike_fi/) | Shrike Fi | ESP32-S3 | `shrike_fi/ntp_clock.ino` |

> Shrike Lite is not supported (no WiFi on RP2040).

## Features

- NTP time sync on boot via `pool.ntp.org`
- Day of week, date (DD-MM-YYYY), and large HH:MM:SS display
- Seconds progress bar at the bottom
- Rounded border frame
- IST timezone offset pre-configured (configurable)

## Wiring

### Shrike Fi (ESP32-S3)

| Function | Signal | GPIO |
|----------|--------|------|
| OLED MOSI | ESP_IO35 | 35 |
| OLED CLK | ESP_IO36 | 36 |
| OLED DC | ESP_IO37 | 37 |
| OLED RST | ESP_IO38 | 38 |
| OLED CS | ESP_IO34 | 34 |
| OLED VCC | 3.3V | — |
| OLED GND | GND | — |

## Dependencies

Install these via the Arduino Library Manager:

| Library | Author |
|---------|--------|
| **Adafruit SSD1306** | Adafruit |
| **Adafruit GFX** | Adafruit |

## Arduino IDE Setup

1. **Board**: `ESP32S3 Dev Module` (ESP32 board package)
2. Edit `ssid` and `password` at the top of the sketch
3. Adjust `gmtOffset_sec` if you're not in IST (default: 19800 for +5:30)
4. Upload to your Shrike Fi

## Usage

1. Power on — the OLED shows "Connecting WiFi..." then "Syncing NTP..."
2. Once synced, the clock runs continuously and updates every second
