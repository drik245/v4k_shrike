# Project 33: MQTT Smart Home Node
Publishes live DHT22 temperature and humidity data to a local MQTT broker (like Mosquitto or Home Assistant).

## Hardware Required
- Shrike Fi (ESP32-S3)
- DHT22 Sensor on `ESP_IO14`

## Software Setup
1. Install `PubSubClient` and `DHT sensor library` in Arduino IDE.
2. Update Wi-Fi and MQTT Broker IP in code.
3. Flash and open Serial Monitor!
