# Project 31: Web-based Oscilloscope

This project turns the **Shrike Fi (ESP32-S3)** into a live, high-speed, web-based oscilloscope. By reading an analog voltage from an ADC pin, it streams the data directly to your web browser via **WebSockets**. The browser renders a live chart using `Chart.js` updating at 25 frames per second!

This is an incredibly useful template for any project where you need to visualize live sensor data (like audio signals, heart rates, or rapid voltage changes) with zero noticeable latency.

## Hardware Required
- Shrike Fi (ESP32-S3)
- A Potentiometer, LDR, or any analog sensor
- **Wiring:** Connect the analog output of your sensor to `ESP_IO4` (or any other ADC pin you choose in the code). Don't forget to connect VCC (3.3V) and GND to the sensor.

## Software Setup (Arduino IDE)

### 1. Install Required Libraries
You need to install the WebSocket library to handle the fast data streaming:
1. Open the Arduino IDE.
2. Go to **Sketch > Include Library > Manage Libraries...**
3. Search for **"WebSockets"** (by Markus Sattler) and click Install.

### 2. Flashing the Code
1. Open `web_oscilloscope.ino` in the Arduino IDE.
2. Update the `ssid` and `password` variables at the top of the file with your Wi-Fi credentials.
3. Select the **ESP32S3 Dev Module** board, compile, and upload!

## How to Use
1. Open the Arduino **Serial Monitor** (115200 baud).
2. Once connected to Wi-Fi, the ESP32 will print an **IP Address** (e.g., `192.168.1.10`).
3. Type that exact IP address into any web browser on your phone or PC (must be on the same Wi-Fi network).
4. You will see a sleek dark-mode chart. Twist your potentiometer or trigger your sensor, and watch the line chart instantly react!
