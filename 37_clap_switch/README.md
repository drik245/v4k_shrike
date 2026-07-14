# Project 37: Clap Toggle Switch

A classic home automation project! This uses a digital sound sensor (microphone module) to detect a loud noise (like a clap) and toggles a relay on or off. You can use this to turn a lamp, fan, or any high-power device on and off just by clapping your hands.

This project is fully cross-board compatible. You can flash the exact same code to either the Shrike Lite or Shrike Fi.

## Hardware Required
- Shrike Lite or Shrike Fi
- Digital Sound Sensor (Microphone module with a digital OUT pin, often uses an LM393 comparator)
- 5V Relay Module
- *(Optional)* A lamp or device to connect to the relay

## Wiring / Pinout
*The code automatically adapts to whichever board you select in the Arduino IDE!*

| Component | Shrike Lite (RP2040) | Shrike Fi (ESP32-S3) |
|-----------|----------------------|----------------------|
| **Sound Sensor OUT** | `RP_IO14` | `ESP_IO14` |
| **Relay IN** | `RP_IO15` | `ESP_IO7` |

*(Don't forget to connect VCC (3.3V or 5V depending on your modules) and GND to both the sensor and the relay!)*

## Software Setup (Arduino IDE)
1. Navigate into either the `shrike_fi` or `shrike_lite` folder, depending on your board.
2. Open the respective `.ino` file in the Arduino IDE.
3. Select the correct board from the boards menu (`ESP32S3 Dev Module` or `Raspberry Pi Pico`).
4. Compile and upload!

## How to Use
1. Adjust the small blue potentiometer on your sound sensor module until the built-in LED turns off, but turns on when you clap. This sets the threshold for what it considers a "loud" sound.
2. Clap your hands once! The relay should click and turn ON.
3. Clap again, and the relay will turn OFF.
4. *(Note: The code has a 500ms debounce delay built-in to prevent a single clap from triggering it multiple times due to echoes).*
