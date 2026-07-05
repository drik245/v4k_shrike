# Project 22: BLE LED Control

This MicroPython project turns your Shrike Fi (ESP32-S3) into a Bluetooth Low Energy (BLE) peripheral. It implements the standard **Nordic UART Service (NUS)**, allowing you to send text commands from your smartphone to control an LED and request status updates.

## Hardware Setup

| Component | Shrike Fi Pin | Description |
|-----------|---------------|-------------|
| LED Anode (+)| ESP_IO21 (GPIO 21) | The LED we will control over BLE |
| LED Cathode (-)| GND | via a current limiting resistor (e.g., 220Ω) |

## Visual Indicators

The LED acts as both the controlled device and a status indicator:
- **Blinking:** The Shrike Fi is advertising and waiting for a Bluetooth connection.
- **Solid ON:** A device (like your phone) is successfully connected.

## How to Test

To interact with the board, you need a BLE terminal app on your phone. We recommend **nRF Connect for Mobile** (available on iOS and Android).

1. **Flash the Code:** Upload `shrike_fi/main.py` to your Shrike Fi board.
2. **Open nRF Connect:** Open the app on your smartphone and scan for devices.
3. **Connect:** Look for a device named **`ShrikeFi-BLE`** and tap `Connect`.
4. **Find the UART Service:** Expand the `Nordic UART Service` (UUID: `6E400001-...`).
5. **Enable Notifications:** Tap the multiple-arrows icon (or similar) next to the **RX Characteristic** to enable notifications. This allows the app to receive messages *from* the Shrike Fi.
6. **Send Commands:** Tap the up-arrow icon next to the **TX Characteristic** to write a value. Make sure the format is set to `UTF-8` (text).

### Supported Commands

Send the following exact strings:
- `LED_ON` — Turns the LED on.
- `LED_OFF` — Turns the LED off.
- `STATUS` — The board will reply back with a message (e.g., "Status: Everything is running smoothly!").

## How it Works

The code uses the `bluetooth` module in MicroPython to create a GATT Server. It registers the Nordic UART Service UUIDs. When a central device (your phone) connects and writes data to the TX characteristic, an interrupt (`_IRQ_GATTS_WRITE`) is triggered. The code reads the byte buffer, decodes it into a string, and toggles the GPIO pin accordingly.
