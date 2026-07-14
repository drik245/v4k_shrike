# Project 32: Bluetooth-Based Wireless LED Control System

*A simple yet powerful MicroPython project to control an LED wirelessly using an HC-05 Bluetooth module!*

This project demonstrates wireless communication and UART data handling by allowing you to turn an LED ON/OFF wirelessly using your smartphone.

## Hardware Required
- Shrike Lite
- HC-05 (Classic Bluetooth) OR HM-10 / AT-09 / ZS-040 (Bluetooth Low Energy) Module
- 1x LED (with a 330Ω resistor)

## Wiring / Pinout

| Component | Shrike Lite (RP2040) |
|-----------|----------------------|
| **HC-05 TXD** | `RP_IO17` (UART0 RX) |
| **HC-05 RXD** | `RP_IO16` (UART0 TX) |
| **LED Anode (+)** | `RP_IO14` |

*(Don't forget to connect VCC (+5V) and GND to the HC-05 module, and connect the LED cathode to GND!)*

## Software Setup (MicroPython)
This project is written in **MicroPython**.

1. Flash your board with MicroPython if you haven't already.
2. Navigate into the `shrike_lite` folder.
3. Upload `main.py` to your board using `mpremote`:
   ```bash
   mpremote cp main.py :main.py
   ```
4. Soft reboot your board to run it:
   ```bash
   mpremote soft-reset
   ```

## How to Connect & Test

### If using an HC-05 (Classic Bluetooth)
1. Open your device's standard Bluetooth Settings menu.
2. Pair with the module (PIN is usually `1234` or `0000`).
3. Download a standard "Serial Bluetooth Terminal" app and connect to it.

### If using an HM-10 / BT05 / ZS-040 (Bluetooth Low Energy / BLE)
*BLE modules cannot be paired in your standard OS settings menu!*
- **On a Laptop:** Use a Web Bluetooth terminal like [loginov-rocks.github.io/Web-Bluetooth-Terminal/](https://loginov-rocks.github.io/Web-Bluetooth-Terminal/) in Chrome or Edge. Click Connect and select your module.
- **On a Phone:** Use an app like "Serial Bluetooth Terminal" (Android) or "BLE Terminal" (iOS). In the app, switch to the **Bluetooth LE (BLE)** tab, scan, and connect directly!

Once connected, simply send a `1` or `on` to turn the LED on, and `0` or `off` to turn it off!
