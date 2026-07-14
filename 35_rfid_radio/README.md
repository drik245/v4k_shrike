# Project 35: RFID Internet Radio Player

Combine the power of the MFRC522 RFID reader with the TDA2030 Internet Radio! Scan physical RFID tags (like cards or keyfobs) to instantly switch between internet radio streams. This creates a tactile "jukebox" experience for your Shrike Fi.

*Note: Since this requires Wi-Fi and audio streaming, it is only compatible with the Shrike Fi (ESP32-S3).*

## Hardware Required
- Shrike Fi (ESP32-S3)
- MFRC522 RFID Module
- TDA2030 Audio Amplifier Module (with speaker)

## Wiring / Pinout

| Component | Shrike Fi (ESP32-S3) |
|-----------|----------------------|
| **RFID SDA (SS)** | `ESP_IO6` |
| **RFID SCK**      | `ESP_IO36` (SPI2_CLK) |
| **RFID MOSI**     | `ESP_IO35` (SPI2_MOSI) |
| **RFID MISO**     | `ESP_IO37` (SPI2_MISO) |
| **RFID RST**      | `ESP_IO5` |
| **TDA2030 IN**    | `ESP_IO4` (I2S DOUT) |

*(Don't forget 3.3V and GND for the RFID, and power/GND for the amplifier! Remember to add the RC filter between IO4 and the amplifier input as detailed in Project 29).*

## Software Setup (Arduino IDE)
1. Install **"MFRC522"** by GithubCommunity and **"ESP32-audioI2S"** by Schreibfaul1 via the Library Manager.
2. Navigate into the `shrike_fi` folder and open `shrike_fi.ino`.
3. Enter your Wi-Fi credentials.
4. Select `ESP32S3 Dev Module` and flash.

## How to Use
1. Flash the code and open the Serial Monitor (115200 baud).
2. Tap any RFID card/fob to the reader.
3. The Serial Monitor will print `Card Scanned: XX XX XX XX`.
4. Copy that UID and update the `tag1` or `tag2` variables in the code.
5. Re-flash the code. Now, whenever you tap that specific card, it will play the assigned internet radio station URL!
