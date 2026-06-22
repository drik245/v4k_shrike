# 👀 Smart UI Eyes with RoboEyes

An interactive **ESP32-S3 + OLED (SSD1306)** smart UI system that runs smoothly animated robot eyes on the idle screen using the [FluxGarage RoboEyes](https://github.com/FluxGarage/RoboEyes) library.

It features touch-based navigation, real-time clock via NTP, and live weather data using the OpenWeatherMap API.

## Board Variants

| Variant | Board | MCU | Sketch |
|---------|-------|-----|--------|
| [shrike_fi](shrike_fi/) | Shrike Fi | ESP32-S3 | `shrike_fi/smart_ui_eyes.ino` |

> Shrike Lite is not supported out-of-the-box in this sketch (requires WiFi for NTP and Weather).

## Features

- **Animated Eyes (Idle Screen)**
  - Smoothly blinking eyes with random idle eye movement
  - Tapping "NEXT" triggers a confused animation
- **Welcome Screen**
  - Displays a connection splash screen on boot while syncing WiFi and Time
- **Dual Touch Control (TTP223 or similar digital touch sensors)**
  - **Touch NEXT** — Navigate menu / Sync time / Trigger animation
  - **Touch SELECT** — Select option / Go back
- **Live Date & Time**
  - NTP-based internet time sync
- **Live Weather Info**
  - Fetches real-time weather using **OpenWeatherMap API**
  - Displays temperature, humidity, and condition
  - Default city: Delhi, IN (configurable in code)
- **State-Based UI Flow**
  - Welcome → Eyes (Idle) → Menu → Time / Weather → Back

## Wiring

### Shrike Fi (ESP32-S3)

| Function | Signal | GPIO |
|----------|--------|------|
| Touch NEXT (TTP223) | ESP_IO1 | 1 |
| Touch SELECT (TTP223) | ESP_IO2 | 2 |
| OLED MOSI | ESP_IO35 | 35 |
| OLED CLK | ESP_IO36 | 36 |
| OLED DC | ESP_IO37 | 37 |
| OLED RST | ESP_IO38 | 38 |
| OLED CS | ESP_IO34 | 34 |
| OLED VCC | 3.3V | — |
| OLED GND | GND | — |

> Touch sensors are read as digital inputs (HIGH when touched). TTP223 modules work out of the box.

## Dependencies

Install these via the Arduino Library Manager:

| Library | Author |
|---------|--------|
| **FluxGarage RoboEyes** | Dennis Hoelscher |
| **Adafruit SSD1306** | Adafruit |
| **Adafruit GFX** | Adafruit |
| **ArduinoJson** | Benoit Blanchon |

## Arduino IDE Setup

1. **Board**: `ESP32S3 Dev Module` (ESP32 board package)
2. Open `shrike_fi/smart_ui_eyes.ino`
3. Edit your `ssid`, `password`, and `apiKey` (OpenWeatherMap) at the top of the file
4. Upload to your Shrike Fi

## Usage

1. Wait for the welcome screen to finish connecting to WiFi and fetching initial data.
2. The eyes will appear and animate automatically (blinking and looking around).
3. **Tap NEXT** to make the eyes play a confused animation.
4. **Tap SELECT** to open the main menu.
5. In the menu, use **NEXT** to cycle through `Clock` and `Weather`, and **SELECT** to open them.
6. Inside the views, **NEXT** refreshes the data (syncs NTP or fetches new weather) and **SELECT** goes back to the eyes.
