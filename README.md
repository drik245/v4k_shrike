# v4k_shrike

Example projects for the Vicharak Shrike development boards — **Shrike Lite (RP2040)** and **Shrike Fi (ESP32-S3)** — featuring an on-board Renesas Forge SLG47910 FPGA.

## Project List

| # | Project | Board / Components | Description |
|---|---------|--------------------|-------------|
| 1 | [Snake Game](snake_game/) | Shrike Lite / Shrike Fi, Joystick Module, SSD1306 OLED | Classic snake game on SSD1306 OLED with joystick control |
| 2 | [BLE Joystick Mouse](joystick_mouse/) | Shrike Fi, Joystick Module | BLE HID mouse using a joystick — requires Bluetooth |
| 3 | [Servo Ctrl with OLED Gauge](servo_ctrl/) | Shrike Lite / Shrike Fi, Joystick Module, Servo Motor | Joystick-controlled servo with a live needle gauge on OLED |
| 4 | [DC Motor Web Speed Control](dc_motor_web/) | Shrike Fi, L298N Driver, DC Motor, **FPGA** | FPGA hardware PWM + Wi-Fi web slider for motor speed/direction |
