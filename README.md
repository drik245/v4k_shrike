# v4k_shrike

Example projects for the Vicharak Shrike development boards — **Shrike Lite (RP2040)** and **Shrike Fi (ESP32-S3)** — featuring an on-board Renesas Forge SLG47910 FPGA.

## Project List

| # | Project | Board / Components | Description |
|---|---------|--------------------|-------------|
| 1 | [Snake Game](01_snake_game/) | Shrike Lite / Shrike Fi, Joystick Module, SSD1306 OLED | Classic snake game on SSD1306 OLED with joystick control |
| 2 | [BLE Joystick Mouse](02_joystick_mouse/) | Shrike Fi, Joystick Module | BLE HID mouse using a joystick — requires Bluetooth |
| 3 | [Servo Ctrl with OLED Gauge](03_servo_ctrl/) | Shrike Lite / Shrike Fi, Joystick Module, Servo Motor | Joystick-controlled servo with a live needle gauge on OLED |
| 4 | [RGB Potentiometer](04_rgb_potentiometer/) | Shrike Lite / Shrike Fi, RGB LED, 3x Potentiometers | (MicroPython) Hardware-agnostic RGB LED color mixer using three analog potentiometers and an EMA noise filter |
| 5 | [DHT22 Web Server](05_dht22_web_server/) | Shrike Fi, DHT22 Sensor | Wi-Fi AP serving temperature and humidity webpage |
| 6 | [ClimaPixel Weather](06_climapixel_weather/) | Shrike Fi, SSD1306 OLED | Wi-Fi Weather display using OpenWeatherMap API |
| 7 | [Blynk IoT Relay](07_blynk_relay/) | Shrike Fi, Relay Module, DHT22 | Cloud-controlled thermostat/relay. Sends temp/humidity to Blynk and uses temperature thresholds to toggle relay. |
| 8 | [NTP Digital Clock](08_ntp_clock/) | Shrike Fi, SSD1306 OLED | Wi-Fi connected digital clock synced to NTP servers |
| 9 | [Telegram Alert](09_telegram_alert/) | Shrike Fi, IR Sensor | Sends Telegram messages when motion is detected |
| 10 | [Ultrasonic OLED](10_ultrasonic_distance/) | Shrike Lite / Shrike Fi, HC-SR04, SSD1306 OLED | Measures distance using ultrasonic pulses and displays on OLED |
| 11 | [MQ135 Gas Sensor](11_mq135_gas_sensor/) | Shrike Lite / Shrike Fi, MQ-135 | Reads air quality using the onboard ADC |
| 12 | [Touch LED Control](12_touch_led_control/) | Shrike Lite / Shrike Fi, Touch Sensor | Toggles the onboard LED using capacitive/digital touch |
| 13 | [Smart UI Eyes](13_smart_ui_eyes/) | Shrike Fi, SSD1306 OLED | Animated robotic eyes displaying weather data |
| 14 | [NTP TM1637 Clock](14_ntp_tm1637_clock/) | Shrike Fi, TM1637 Display | NTP-synced HH:MM clock on a 4-digit seven-segment display |
| 15 | [Button Press Counter](15_button_counter/) | Shrike Lite / Shrike Fi, TM1637 Display, Button | Button counter on TM1637 — short press to count, hold to reset |
| 16 | [Alarm Clock](16_alarm_clock/) | Shrike Fi, TM1637 Display, Buttons, Buzzer | Offline alarm clock — manual time set, blinking digits, buzzer alert |
| 17 | [Mini Weather Station](17_mini_weather_station/) | Shrike Fi, 16x2 LCD, DHT22 Sensor | Localized weather station displaying data on a 4-bit parallel LCD |
| 18 | [Smart IR Object Detection](18_ir_object_detection/) | Shrike Lite / Shrike Fi, IR Sensor, LED, Buzzer | Simple object detection system with visual and auditory feedback |
| 19 | [Wi-Fi 4WD Robot](19_wifi_4wd_robot/) | Shrike Fi, L298N, Servo, HC-SR04 | A massive 16-pin project: Wi-Fi controlled car with speed control and obstacle avoidance |
| 20 | [Finger Gesture LEDs](20_finger_gesture_leds/) | Shrike Lite / Shrike Fi, OpenCV, MediaPipe | (MicroPython) Detects hand gestures via PC webcam and toggles LEDs |
| 21 | [Auto Night Light](21_Auto_Night_Light/) | Shrike Lite / Shrike Fi, LDR, LED | Uses LDR to detect brightness and triggers a smooth LED breathing effect using 12-bit PWM |
| 22 | [BLE LED Control](22_ble_led_control/) | Shrike Fi | (MicroPython) Control an LED via Bluetooth Low Energy using nRF Connect app |
| 23 | [OTA GitHub Update](23_ota_github_update/) | Shrike Fi | (MicroPython) Over-the-air code updates from GitHub on every boot |
| 24 | [RFID Access Control](24_rfid_access_control/) | Shrike Lite / Shrike Fi, MFRC522 | (MicroPython) Scans RFID cards and grants/denies access based on authorized UID list |
| 25 | [RFID Attendance Logger](25_rfid_attendance_logger/) | Shrike Fi, MFRC522 | (MicroPython) Logs RFID attendance to Google Sheets via Wi-Fi webhook |
| 26 | [ESP-NOW RFID Display](26_espnow_rfid_display/) | Shrike Fi + ESP32 WROOM, MFRC522, OLED | (MicroPython) Wirelessly sends scanned RFID UIDs to a second board's OLED via ESP-NOW |
| 27 | [ESP-NOW Home Automation](27_espnow_home_automation/) | Shrike Fi + ESP32 WROOM, 4-Ch Relay, DHT22, OLED | (MicroPython) Bidirectional ESP-NOW: 4 buttons control remote relays, sensor data sent back to OLED |
| 28 | [PIR Parking System](28_pir_parking_system/) | Shrike Lite / Shrike Fi, 2x PIR, OLED | (MicroPython) Monitors 2 parking slots with PIR sensors and shows live status on OLED |
| 29 | [OLED Internet Radio](29_internet_radio/) | Shrike Fi, SSD1306 OLED, TDA2030 Amplifier, 3.5mm Jack, Buttons | Standalone internet radio with mono audio output via PDM, OLED with WiFi signal bars, and button-based station navigation |
| 30 | [RGB LED Control using Python GUI](30_RGB_Led_Ctrl_using_pythonGUI/) | Shrike Lite / Shrike Fi, RGB LED | (MicroPython/Python) Full-stack RGB LED control! Custom PC GUI communicates over USB serial to control an RGB LED with breath, disco, and cycle modes |

## Getting Started

For board-specific documentation, pinouts, and firmware downloads, check the official Shrike repo:
- **[Vicharak Shrike GitHub](https://github.com/vicharak-in/shrike)** — Getting started guides, hardware files, and UF2 firmware for Shrike Lite, Shrike Fi, and Shrike.

### Arduino IDE Projects
Most projects in this repo use the **Arduino IDE**. Select the correct board in the IDE:
- **Shrike Lite** → Board: `Raspberry Pi Pico`
- **Shrike Fi** → Board: `ESP32S3 Dev Module`

### MicroPython Projects
Projects marked with `(MicroPython)` run on MicroPython firmware. You'll need **mpremote** to manage files on the board.

## mpremote Quick Reference

### Install
```bash
pip install mpremote
```

### Basic Commands

| What | Command |
|------|---------|
| Open REPL | `mpremote repl` |
| List connected devices | `mpremote devs` |
| Soft reset | `mpremote soft-reset` |
| Run a local script (without uploading) | `mpremote run script.py` |

### File Management
Use `:` as prefix to refer to files on the board.

```bash
# list files on the board
mpremote fs ls

# copy a file to the board
mpremote cp main.py :main.py

# copy a file from the board to your PC
mpremote cp :main.py ./backup.py

# delete a file on the board
mpremote rm :old_file.py
```

### Tips
- MicroPython auto-runs `main.py` on boot. Name your script `main.py` or create a `main.py` that imports it.
- To run a script for testing without saving it to the board, use `mpremote run script.py`.
- If `mpremote` can't connect ("could not enter raw repl"), your `main.py` is probably running and blocking. Hold the **BOOT** button and replug USB to enter bootloader mode, then re-flash MicroPython.
- Chain commands with `+`: `mpremote cp main.py :main.py + soft-reset`
