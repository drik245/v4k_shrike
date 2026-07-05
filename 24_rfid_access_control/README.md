# Project 24: RFID Access Control

This MicroPython project turns your Shrike board into a secure RFID access control system. It uses an MFRC522 module to scan RFID tags/cards and compares the unique ID (UID) against a hardcoded list of authorized users.

If the UID is recognized, it prints "ACCESS GRANTED" and flashes a Green LED. If the UID is unknown, it prints "ACCESS DENIED" and flashes a Red LED.

## Hardware Wiring

The MFRC522 communicates over SPI. To ensure maximum stability, we use Hardware SPI rather than Software SPI.

### Shrike Fi (ESP32-S3)
| Component | MFRC522 Pin | Shrike Fi Pin |
|-----------|-------------|---------------|
| SCK | SCK | ESP_IO5 |
| MOSI | MOSI | ESP_IO6 |
| MISO | MISO | ESP_IO7 |
| CS / SDA | SDA | ESP_IO4 |
| Reset | RST | ESP_IO3 |
| Green LED | Anode (+) | ESP_IO1 |
| Red LED | Anode (+) | ESP_IO2 |
| Power | 3.3V | 3.3V (Do **not** use 5V for MFRC522) |

### Shrike Lite (RP2040)
| Component | MFRC522 Pin | Shrike Lite Pin |
|-----------|-------------|-----------------|
| SCK | SCK | RP_IO18 |
| MOSI | MOSI | RP_IO19 |
| MISO | MISO | RP_IO16 |
| CS / SDA | SDA | RP_IO17 |
| Reset | RST | RP_IO20 |
| Green LED | Anode (+) | RP_IO15 |
| Red LED | Anode (+) | RP_IO14 |
| Power | 3.3V | 3.3V (Do **not** use 5V for MFRC522) |

> **Note on SPI Stability:** The `mfrc522.py` driver has been specifically configured to use Hardware SPI at 1 MHz. Do not use Software SPI or higher baud rates as it can lead to dropped bits or silent failures when reading tags.

## Usage Guide

1. **Upload the Files:** Copy `main.py` and `mfrc522.py` from the folder matching your board to the board's filesystem (e.g., using `mpremote cp`).
2. **First Run (Adding Authorized Tags):**
   - Run the script and open the REPL monitor.
   - Scan an unauthorized card. It will print the UID (e.g., `Card UID: 0B:0F:2D:07`) and say `ACCESS DENIED`.
   - Copy that UID.
3. **Authorize the Card:**
   - Open `main.py` and find the `AUTHORIZED` list at the top.
   - Add the copied UID to the list as a string.
   - Re-upload `main.py` to your board.
4. **Test Access:**
   - Scan the card again. The system will recognize it, print `ACCESS GRANTED`, and flash the Green LED!
