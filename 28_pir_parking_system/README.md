# Project 28: PIR Parking System

This project is a smart parking slot monitor using two HC-SR501 PIR sensors and an SSD1306 OLED display. It tracks motion to detect if a car has parked in the slot or if it's free. 

The project supports both the **Shrike Fi (ESP32-S3)** and **Shrike Lite (RP2040)** boards.

## Features
- Independent monitoring of 2 parking slots.
- Real-time status display on a 128x64 OLED screen over SPI.
- Includes a dedicated `test_pir.py` script for hardware debugging.

## Wiring Guide

### Shrike Fi (ESP32-S3)
* **PIR 1:** `ESP_IO14`
* **PIR 2:** `ESP_IO2`
* **OLED SCK:** `ESP_IO5`
* **OLED MOSI:** `ESP_IO6`
* **OLED CS:** `ESP_IO7`
* **OLED DC:** `ESP_IO4`
* **OLED RES:** `ESP_IO3`
* **Dummy MISO:** `ESP_IO1` *(Required to prevent internal SPI hijacking)*

### Shrike Lite (RP2040)
* **PIR 1:** `RP_IO16`
* **PIR 2:** `RP_IO17`
* **OLED SCK:** `RP_IO10`
* **OLED MOSI:** `RP_IO11`
* **OLED CS:** `RP_IO14`
* **OLED DC:** `RP_IO15`
* **OLED RES:** `RP_IO9`
* **Dummy MISO:** `RP_IO8` *(Required to prevent internal SPI hijacking)*

## Troubleshooting PIR Sensors
HC-SR501 sensors often trigger continuously or hold a `HIGH` signal for too long when first powered up. 
- **Time Delay:** Turn the left potentiometer on the back of the sensor fully counter-clockwise to reduce the delay to its minimum (~2-3 seconds).
- **Testing:** Run the `test_pir.py` script from your terminal using `mpremote run test_pir.py` to see the raw digital output of the sensors.
