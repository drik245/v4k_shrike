# Project 27: ESP-NOW Home Automation

This project demonstrates **bi-directional** ESP-NOW communication.
* **The Sender (Shrike Fi):** Has 4 hardware buttons and an SPI OLED display. Pressing a button sends a command to toggle a relay on the receiver. It also listens for sensor updates from the receiver and displays the live Temperature and Humidity on the screen.
* **The Receiver (ESP32 WROOM):** Controls a 4-channel relay module and reads a DHT22 temperature/humidity sensor. It receives toggle commands, triggers the physical relays, and periodically broadcasts the sensor readings back to the sender.

## Hardware Wiring

### Sender: Shrike Fi (ESP32-S3)
| Component | Pin | Shrike Fi Header |
|-----------|-----|------------------|
| Button 1 | One leg to GND, other to Pin | ESP_IO1 |
| Button 2 | One leg to GND, other to Pin | ESP_IO2 |
| Button 3 | One leg to GND, other to Pin | ESP_IO3 |
| Button 4 | One leg to GND, other to Pin | ESP_IO4 |
| OLED SCK | SCK / D0 | ESP_IO5 |
| OLED MOSI | SDA / D1 | ESP_IO6 |
| OLED CS | CS | ESP_IO7 |
| OLED DC | DC | ESP_IO8 |
| OLED RES | RES | ESP_IO9 |
*(Buttons use internal pull-ups. No external resistors needed).*

### Receiver: ESP32 WROOM
| Component | Pin | ESP32 WROOM Pin |
|-----------|-----|-----------------|
| Relay 1 | IN1 | GPIO 16 |
| Relay 2 | IN2 | GPIO 17 |
| Relay 3 | IN3 | GPIO 18 |
| Relay 4 | IN4 | GPIO 19 |
| DHT22 Sensor | DATA | GPIO 4 |
*(Most 4-channel relay boards require an external 5V supply to switch reliably. Use a 5V source for the relay's VCC/JD-VCC).*

---

## Setting up the ESP32 WROOM (Flashing MicroPython)

Both boards run MicroPython. If your ESP32 WROOM doesn't have MicroPython installed yet, you can flash it using the included `ESP32.bin` file (downloaded from the [official ESP32 Generic release page](https://micropython.org/download/ESP32_GENERIC/)).

Open a terminal in this project folder and run the following commands (replace `COMxx` with your actual COM port, e.g., `COM4`):

**1. Erase the existing firmware:**
```bash
python -m esptool --chip esp32 --port COMxx erase_flash
```

**2. Flash the new MicroPython firmware (starts at 0x1000 on standard ESP32):**
```bash
python -m esptool --chip esp32 --port COMxx -b 460800 --before default_reset --after hard_reset write_flash -z --flash_mode dio --flash_size 4MB --flash_freq 80m 0x1000 ESP32.bin
```

---

## Running the Project

### 1. Get the Receiver's MAC Address
ESP-NOW requires the Sender to know the exact hardware address of the Receiver. 
1. Connect your ESP32 WROOM Receiver.
2. Upload and run the helper script:
   ```bash
   python -m mpremote cp get_mac.py :main.py
   python -m mpremote reset
   ```
3. The terminal will print a formatted string like `b'\x00\x4B\x12\x3B\x25\x48'`. Copy this!

### 2. Configure and Flash the Sender
1. Open `sender/main.py`.
2. Find `RECEIVER_MAC = b'\xFF\xFF\xFF\xFF\xFF\xFF'` and replace it with the MAC address you just copied.
3. Upload the code to your **Shrike Fi**:
   ```bash
   cd sender
   python -m mpremote cp main.py :main.py
   ```
*(Note: If your Shrike Fi doesn't have the `ssd1306.py` library installed yet, you'll need to upload that too).*

### 3. Flash the Receiver
1. Connect your **ESP32 WROOM**.
2. Upload the receiver code:
   ```bash
   cd receiver
   python -m mpremote cp main.py :main.py
   ```

### 4. Test it!
Power both boards on. The Shrike Fi OLED will light up.
- Press buttons 1-4 on the Shrike Fi to toggle the relays on the ESP32 WROOM.
- Every 2 seconds, the ESP32 WROOM will read the DHT22 sensor and blast the readings back to the Shrike Fi, updating the OLED display instantly!

---
## 💡 MicroPython Tip: Auto-run on Boot
By default, MicroPython boards automatically search for and execute a file named **`main.py`** on boot. If your code is inside a file with a different name (e.g., `app.py`), it will **not** run automatically when you power on the board.

To make it run on boot, you have two options:
1. Rename your script to `main.py`.
2. Create a `main.py` file that simply imports your script:
   ```python
   # Inside main.py
   import app
   ```
