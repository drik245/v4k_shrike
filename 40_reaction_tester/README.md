# Project 40: Reaction Time Tester Game

Measure your reflexes in milliseconds! The OLED will tell you to wait for a random amount of time (2 to 6 seconds). Suddenly, the screen will flash "PRESS NOW!" and an LED will light up. Hit the button as fast as you can, and the screen will display your exact reaction time in milliseconds.

If you try to cheat and press the button before the LED turns on, the screen will catch you and yell "TOO EARLY!".

This project is fully cross-board compatible. You can flash the exact same code to either the Shrike Lite or Shrike Fi.

## Hardware Required
- Shrike Lite or Shrike Fi
- SSD1306 OLED Display (I2C)
- 1x Push Button
- 1x LED (with a 220Ω or 330Ω resistor)

## Wiring / Pinout
*The code automatically adapts to whichever board you select in the Arduino IDE!*

| Component | Shrike Lite (RP2040) | Shrike Fi (ESP32-S3) |
|-----------|----------------------|----------------------|
| **OLED SDA** | `RP_IO6` | `ESP_IO7` |
| **OLED SCL** | `RP_IO7` | `ESP_IO6` |
| **LED Anode (+)** | `RP_IO14` | `ESP_IO14` |
| **Button Pin** | `RP_IO15` | `ESP_IO5` |

*(Note: Connect the other side of the button to `GND`. No external resistor is needed since we use `INPUT_PULLUP`. Connect the LED Cathode to GND via a resistor.)*

## Software Setup (Arduino IDE)
1. Open the Arduino IDE.
2. Go to **Sketch > Include Library > Manage Libraries...**
3. Search for and install **"Adafruit SSD1306"** and **"Adafruit GFX Library"**.
4. Navigate into either the `shrike_fi` or `shrike_lite` folder, depending on your board.
5. Open the respective `.ino` file.
6. Select the correct board (`ESP32S3 Dev Module` or `Raspberry Pi Pico`).
7. Compile and upload!
