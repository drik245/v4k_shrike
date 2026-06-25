# 🚗 Wi-Fi Controlled 4WD Robot Car

A fully featured, Wi-Fi controlled 4-wheel drive robot. The ESP32 hosts its own Access Point (AP) and serves a sleek HTML/CSS/JS web page. You can connect your phone to the robot's Wi-Fi network and drive it using on-screen buttons and a speed slider.

It also features **autonomous obstacle detection** using an Ultrasonic sensor mounted on a scanning servo, and a **reversing buzzer** for that authentic heavy-machinery feel!

## Board Variants

| Variant | Board | MCU | Sketch |
|---------|-------|-----|--------|
| [shrike_fi](shrike_fi/) | Shrike Fi | ESP32-S3 | `shrike_fi/wifi_4wd_robot.ino` |

> Shrike Lite is not supported (no Wi-Fi, and not enough GPIO headers without multiplexing).

## Features

- **Wi-Fi Access Point**: Creates an isolated network (`Shrike-4WD`) so you can drive it anywhere.
- **Web UI**: Serves a mobile-friendly interface with directional buttons (Hold to move, release to stop).
- **Speed Control (PWM)**: Real-time speed adjustment via a web slider.
- **Scanning Ultrasonic Sensor**: A servo continuously sweeps the HC-SR04 back and forth. If an object is detected < 15cm away, the robot automatically emergency stops.
- **Reverse Buzzer**: Beeps when driving backwards, and beeps rapidly if blocked by an obstacle.

## Wiring (16 GPIOs Total!)

This project uses a massive amount of IO! Make sure your breadboard routing is clean. 
**CRITICAL**: You MUST connect the GND of both L298N motor drivers to the Shrike Fi GND.

### Driver 1 (Front Motors)

| Function | L298N Pin | Shrike Fi GPIO |
|----------|-----------|----------------|
| Front Right Forward | IN1 | ESP_IO1 |
| Front Right Backward| IN2 | ESP_IO2 |
| Front Left Forward  | IN3 | ESP_IO3 |
| Front Left Backward | IN4 | ESP_IO4 |
| Speed Control A     | ENA | ESP_IO5 (PWM) |
| Speed Control B     | ENB | ESP_IO6 (PWM) |

### Driver 2 (Rear Motors)

| Function | L298N Pin | Shrike Fi GPIO |
|----------|-----------|----------------|
| Rear Right Forward  | IN1 | ESP_IO7 |
| Rear Right Backward | IN2 | ESP_IO8 |
| Rear Left Forward   | IN3 | ESP_IO9 |
| Rear Left Backward  | IN4 | ESP_IO10 |
| Speed Control A     | ENA | ESP_IO11 (PWM) |
| Speed Control B     | ENB | ESP_IO12 (PWM) |

### Sensors & Actuators

| Component | Function | Shrike Fi GPIO |
|-----------|----------|----------------|
| HC-SR04 | Trig | ESP_IO13 |
| HC-SR04 | Echo | ESP_IO14 |
| Servo | Signal | ESP_IO15 |
| Buzzer | Positive | ESP_IO16 |

## Dependencies

Install these via the Arduino Library Manager:

| Library | Author |
|---------|--------|
| **ESP32Servo** | Kevin Harrington, John K. Bennett |
| (WiFi and WebServer are built into the ESP32 core) | |

## Usage

1. Assemble the car and double-check the massive wiring harness.
2. Flash `shrike_fi/wifi_4wd_robot.ino` to the Shrike Fi.
3. Power the car via batteries. The Shrike Fi should be powered either by USB power bank or a stable 5V buck converter from the battery.
4. On your phone, connect to the Wi-Fi network: `Shrike-4WD` (Password: `password123`).
5. Open your browser and go to `http://192.168.4.1`.
6. Use the web interface to drive!
