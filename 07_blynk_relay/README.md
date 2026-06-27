# 🌡️ Blynk Smart Thermostat Relay

A cloud-connected smart thermostat built using the **Shrike Fi** (ESP32-S3), a DHT22 temperature & humidity sensor, and a Relay module. It connects to the Blynk IoT platform over Wi-Fi, providing real-time telemetry and remote control.

## 🌟 Features

- **Automated Edge-Triggered Thermostat:** Constantly monitors the room temperature. When the temperature crosses your predefined `TEMP_THRESHOLD` (e.g., 30°C), it automatically triggers the relay (perfect for fans or AC units). 
- **Smart Manual Override:** Because the thermostat is "edge-triggered", it won't fight you! If it's hot and the relay turns ON automatically, you can still press a button to manually turn it OFF, and the system will respect your choice until the temperature crosses the threshold again.
- **Active-Low Support:** Built-in software switch (`#define ACTIVE_LOW_RELAY true`) makes it effortlessly compatible with cheap active-low relay modules without requiring messy hardware inverters.
- **Blynk Cloud Integration:** Sends live temperature and humidity data to your phone, and accepts remote relay commands from anywhere in the world.

## 🔌 Hardware Setup

### Shrike Fi Wiring
| Component | Shrike Fi Pin | Note |
|-----------|---------------|------|
| **Relay Module** | `ESP_IO3` (GPIO 3) | Signal pin to trigger the relay. |
| **Manual Button** | `ESP_IO14` (GPIO 14) | Wire one side to IO14 and the other to GND. Uses internal pull-up. |
| **Status LED** | `ESP_IO21` (GPIO 21) | Optional LED to mirror the relay state. |
| **DHT22 Data** | `ESP_IO4` (GPIO 4) | Sensor data line. |

## 📱 Blynk App Configuration

Set up a Device in your Blynk Web Dashboard/App with the following Datastreams:

1. **Virtual Pin V0:** Data type `Double` or `Float` (Used for live Temperature in °C)
2. **Virtual Pin V1:** Data type `Double` or `Float` (Used for live Humidity in %)
3. **Virtual Pin V2:** Data type `Integer`, Min `0`, Max `1` (Used for the Relay Switch)

> [!WARNING]
> Ensure your V2 switch widget in the Blynk App has a **Max value of 1**. If it is set to 255 (the default for some sliders/buttons), the logic will fail to recognize the "ON" state!

## 🚀 Setup Instructions

1. Install the following libraries in Arduino IDE:
   - `Blynk` by Volodymyr Shymanskyy
   - `DHTesp` (Optimized DHT library for ESP32)
2. Open `shrike_fi/blynk_relay.ino`.
3. Enter your Wi-Fi credentials (`ssid` and `pass`).
4. Enter your Blynk `BLYNK_TEMPLATE_ID`, `BLYNK_DEVICE_NAME`, and `BLYNK_AUTH_TOKEN` at the top of the file.
5. Flash the code and enjoy your smart thermostat!
