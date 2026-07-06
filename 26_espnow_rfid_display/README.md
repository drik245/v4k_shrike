# Project 26: ESP-NOW RFID Display

This project demonstrates wireless communication using **ESP-NOW**, a fast, connectionless Wi-Fi protocol developed by Espressif. 

It uses two boards:
1. **The Sender (Shrike Fi):** Scans an RFID card and instantly broadcasts its UID over the air.
2. **The Receiver (ESP32 WROOM):** Listens for the incoming UID and displays it on an SSD1306 SPI OLED screen.

## Hardware Setup

### Sender: Shrike Fi (ESP32-S3)
*MicroPython environment*

| Component | MFRC522 Pin | Shrike Fi Pin |
|-----------|-------------|---------------|
| SCK | SCK | ESP_IO5 |
| MOSI | MOSI | ESP_IO6 |
| MISO | MISO | ESP_IO7 |
| CS / SDA | SDA | ESP_IO4 |
| Reset | RST | ESP_IO3 |
| Power | 3.3V | 3.3V |

### Receiver: ESP32 WROOM
*Arduino IDE environment*

| Component | SPI OLED Pin | ESP32 WROOM Pin |
|-----------|--------------|-----------------|
| MOSI | SDA / D1 | GPIO 23 (VSPI) |
| CLK | SCL / D0 | GPIO 18 (VSPI) |
| CS | CS | GPIO 5 (VSPI) |
| DC | DC | GPIO 4 |
| Reset | RES | GPIO 2 |

## How to Run

### Step 1: Flash the Receiver
1. Open `receiver_arduino/receiver_arduino.ino` in the Arduino IDE.
2. Install the `Adafruit GFX Library` and `Adafruit SSD1306` via the Library Manager if you haven't already.
3. Flash the code to your ESP32 WROOM.
4. Open the Arduino Serial Monitor (115200 baud). The ESP32 will print its MAC address (e.g., `00:4B:12:3B:25:48`). **Copy this address.**

### Step 2: Configure the Sender
1. Open `sender/main.py`.
2. Locate the `RECEIVER_MAC` variable near the top.
3. Paste the MAC address you copied from the receiver. Format it exactly like this: `b'\x00\x4B\x12\x3B\x25\x48'` (replacing the hexadecimal pairs with your own).
4. Save the file.

### Step 3: Test the System!
1. Upload `main.py` and `mfrc522.py` to your Shrike Fi board.
2. Run `main.py` on the Shrike Fi.
3. Scan an RFID card on the Shrike Fi's reader.
4. The Shrike Fi will print `Sent to receiver: UID:...` and the ESP32 WROOM's OLED display will instantly update with the scanned UID and increment the scan count!
