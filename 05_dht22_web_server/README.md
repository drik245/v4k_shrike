# 🌡️ DHT22 Web Server

WiFi access point + web server showing real-time **temperature**, **humidity**, and **heat index** from a DHT22 sensor. No router needed — the Shrike Fi creates its own WiFi network you connect to directly.

Also exposes a **JSON API** at `/api` for programmatic access.

## Board Variants

| Variant | Board | MCU | Sketch |
|---------|-------|-----|--------|
| [shrike_fi](shrike_fi/) | Shrike Fi | ESP32-S3 | `shrike_fi/dht22_web_server.ino` |

> Shrike Lite is not supported (no WiFi on RP2040).

## Features

- Creates a standalone WiFi access point (no router needed)
- Styled dark-theme web dashboard with auto-refresh every 3 seconds
- Real-time temperature, humidity, and computed heat index
- JSON API endpoint at `/api` for integration with other projects
- LED indicator turns on when AP is ready

## Wiring

### Shrike Fi (ESP32-S3)

| Function | Signal | GPIO |
|----------|--------|------|
| DHT22 Data | ESP_IO4 | 4 |
| Status LED | ESP_IO21 | 21 |
| DHT22 VCC | 3.3V | — |
| DHT22 GND | GND | — |

> A 10K pull-up resistor between DHT22 data and VCC is recommended for reliable readings.

## Dependencies

Install these via the Arduino Library Manager:

| Library | Author |
|---------|--------|
| **DHT sensor library for ESPx** | beegee-tokyo |

## Arduino IDE Setup

1. **Board**: `ESP32S3 Dev Module` (ESP32 board package)
2. Select the correct COM port
3. Open `shrike_fi/dht22_web_server.ino` and upload

## Usage

1. Upload the sketch
2. Connect your phone/laptop to WiFi network **Shrike-Weather** (password: `12345678`)
3. Open `http://192.168.4.1` in a browser
4. Temperature, humidity, and heat index auto-refresh every 3 seconds

### JSON API

```
GET http://192.168.4.1/api
```

Response:
```json
{
  "temperature_c": 28.5,
  "humidity": 65.0,
  "heat_index_c": 30.2
}
```
