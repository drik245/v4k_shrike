# 🕐 NTP TM1637 Clock

Internet-synced digital clock on a **TM1637 4-digit seven-segment display**. Shows HH:MM with a blinking colon, synced via NTP.

## Board Variants

| Variant | Board | MCU | Sketch |
|---------|-------|-----|--------|
| [shrike_fi](shrike_fi/) | Shrike Fi | ESP32-S3 | `shrike_fi/ntp_tm1637_clock.ino` |

> Shrike Lite is not supported (no WiFi on RP2040).

## Features

- NTP time sync on boot via `pool.ntp.org`
- HH:MM display with blinking colon (toggles every 500ms)
- Shows "conn" while connecting to WiFi, "Sync" while waiting for NTP
- Dashes ("----") if time sync is lost
- IST timezone pre-configured (configurable)

## Wiring

### Shrike Fi (ESP32-S3)

| Function | Signal | GPIO |
|----------|--------|------|
| TM1637 CLK | ESP_IO1 | 1 |
| TM1637 DIO | ESP_IO2 | 2 |
| TM1637 VCC | 3.3V | — |
| TM1637 GND | GND | — |

## Dependencies

Install these via the Arduino Library Manager:

| Library | Author |
|---------|--------|
| **TM1637Display** | Avishay Orpaz |

## Arduino IDE Setup

1. **Board**: `ESP32S3 Dev Module`
2. Edit `ssid` and `password` at the top of the sketch
3. Adjust `gmtOffset_sec` if not in IST (default: 19800 for +5:30)
4. Upload to your Shrike Fi
