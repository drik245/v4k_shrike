# v4k_shrike

Example projects for the Vicharak Shrike development boards — **Shrike Lite (RP2040)** and **Shrike Fi (ESP32-S3)** — featuring an on-board Renesas Forge SLG47910 FPGA.

## Project List

| # | Project | Board / Components | Description |
|---|---------|--------------------|-------------|
| 1 | [Snake Game](snake_game/) | Shrike Lite / Shrike Fi, Joystick Module, SSD1306 OLED | Classic snake game on SSD1306 OLED with joystick control |
| 2 | [BLE Joystick Mouse](joystick_mouse/) | Shrike Fi, Joystick Module | BLE HID mouse using a joystick — requires Bluetooth |
| 3 | [Servo Ctrl with OLED Gauge](servo_ctrl/) | Shrike Lite / Shrike Fi, Joystick Module, Servo Motor | Joystick-controlled servo with a live needle gauge on OLED |
| 4 | [DC Motor Web Speed Control](dc_motor_web/) | Shrike Fi, L298N Driver, DC Motor, **FPGA** | FPGA hardware PWM + Wi-Fi web slider for motor speed/direction |
| 5 | [DHT11 Web Server](dht11_web_server/) | Shrike Fi, DHT11 Sensor | Wi-Fi Web Server displaying temperature and humidity |
| 6 | [ClimaPixel Weather](climapixel_weather/) | Shrike Fi, SSD1306 OLED | Wi-Fi Weather display using OpenWeatherMap API |
| 7 | [Blynk IoT Relay](blynk_relay/) | Shrike Fi, Relay Module | Cloud-controlled relay using the Blynk App |
| 8 | [NTP Digital Clock](ntp_clock/) | Shrike Fi, SSD1306 OLED | Wi-Fi connected digital clock synced to NTP servers |
| 9 | [Telegram Alert](telegram_alert/) | Shrike Fi, IR Sensor | Sends Telegram messages when motion is detected |
| 10 | [Ultrasonic OLED](ultrasonic_distance/) | Shrike Lite / Shrike Fi, HC-SR04, SSD1306 OLED | Measures distance using ultrasonic pulses and displays on OLED |
| 11 | [MQ135 Gas Sensor](mq135_gas_sensor/) | Shrike Lite / Shrike Fi, MQ-135 | Reads air quality using the onboard ADC |
| 12 | [Touch LED Control](touch_led_control/) | Shrike Lite / Shrike Fi, Touch Sensor | Toggles the onboard LED using capacitive/digital touch |
| 13 | [Smart UI Eyes](smart_ui_eyes/) | Shrike Fi, SSD1306 OLED | Animated robotic eyes displaying weather data |
| 14 | [NTP TM1637 Clock](ntp_tm1637_clock/) | Shrike Fi, TM1637 Display | NTP-synced HH:MM clock on a 4-digit seven-segment display |
| 15 | [Button Press Counter](button_counter/) | Shrike Lite / Shrike Fi, TM1637 Display, Button | Button counter on TM1637 — short press to count, hold to reset |
