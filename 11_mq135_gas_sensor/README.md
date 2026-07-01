# Project 11: MQ-135 Gas Sensor

This project demonstrates how to use the MQ-135 Air Quality / Gas sensor with the Shrike boards. It reads the analog output (AO) from the sensor and uses a 16-sample averaging technique for stable readings. 

Depending on the gas concentration detected, it categorizes the air quality into `GOOD`, `POOR`, or `DANGER`, and triggers visual and audible alerts.

## Code Versions

There are two versions of the code provided:
- **`shrike_fi/`**: For the ESP32-S3 based Shrike Fi board. Includes support for a warning LED and a buzzer.
- **`shrike_lite/`**: For the RP2040 based Shrike Lite board. Uses the onboard LED for warnings.

## Hardware Wiring

### Shrike Fi (ESP32-S3)
| Component | Shrike Fi Pin | Description |
|---|---|---|
| MQ-135 AO | ESP_IO1 (GPIO 1) | Analog output from the gas sensor |
| LED Anode (+) | ESP_IO21 (GPIO 21) | Warning LED (turns on for POOR/DANGER air) |
| Buzzer (+) | ESP_IO3 (GPIO 3) | Optional buzzer (sounds on DANGER) |
| Power / GND | 5V / GND | Power supply for the MQ-135 heater |

### Shrike Lite (RP2040)
| Component | Shrike Lite Pin | Description |
|---|---|---|
| MQ-135 AO | RP_IO29 (GPIO 29) | Analog output from the gas sensor |
| Onboard LED | RP_IO4 (GPIO 4) | Warning LED (turns on for POOR/DANGER air) |
| Power / GND | 5V / GND | Power supply for the MQ-135 heater |

> **Note:** The MQ-135 sensor has a heating element that requires 5V to operate properly and takes about 20-30 seconds to "warm up" when first powered on before readings become accurate.

## Calibration (Tuning Thresholds)
The analog readings vary depending on the specific sensor, temperature, and humidity. By default, the code uses these thresholds:
- `GOOD_LIMIT = 1000`
- `WARN_LIMIT = 2000`

To calibrate for your environment:
1. Power on the sensor in clean air and let it warm up for a few minutes.
2. Open the Serial Monitor (115200 baud).
3. Observe the `avg` reading.
4. Adjust `GOOD_LIMIT` to be slightly higher than your clean air baseline.
5. Adjust `WARN_LIMIT` to be the threshold where you want the buzzer/danger alert to trigger (you can test this by bringing a lighter or rubbing alcohol near the sensor).

## Usage
1. Open the `.ino` file for your board in the Arduino IDE.
2. Ensure you have the correct board selected (`ESP32S3 Dev Module` for Shrike Fi, or `Raspberry Pi Pico` for Shrike Lite).
3. Upload the code and open the Serial Monitor at `115200` baud.
