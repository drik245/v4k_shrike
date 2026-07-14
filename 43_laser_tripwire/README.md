# Project 43: Laser Tripwire Alarm

Build a classic laser security system! When the laser beam is broken, a blaring alarm siren goes off and latches ON until you press the reset button. Written in **C++ (Arduino)**.

Supports both **Shrike Lite** and **Shrike Fi**.

## Hardware Required
| Component | Qty |
|-----------|-----|
| Shrike Lite or Shrike Fi | 1 |
| Laser Transmitter Module | 1 |
| LDR (Light Dependent Resistor) | 1 |
| 10kΩ Resistor (for LDR pull-down) | 1 |
| Active Buzzer | 1 |
| Momentary Push Button (Reset) | 1 |

## Wiring / Pinout

### Shrike Lite (RP2040)
| Component | Pin |
|-----------|-----|
| LDR Sensor (Voltage Divider) | `RP_IO26` (ADC0) |
| Laser Module Signal | `RP_IO15` |
| Active Buzzer (+) | `RP_IO10` |
| Reset Button | `RP_IO14` → GND |

### Shrike Fi (ESP32-S3)
| Component | Pin |
|-----------|-----|
| LDR Sensor (Voltage Divider) | `ESP_IO1` (ADC_CH0) |
| Laser Module Signal | `ESP_IO7` |
| Active Buzzer (+) | `ESP_IO6` |
| Reset Button | `ESP_IO14` → GND |

**Power connections:** 
- Connect Laser Module VCC to 3.3V, GND to GND.
- Connect Buzzer (-) to GND.
- LDR Voltage Divider: Connect one leg of the LDR to 3.3V. Connect the other leg to the ADC pin AND to GND through a 10kΩ resistor.
- The reset button uses `INPUT_PULLUP`, so just connect one side to the IO pin and the other to GND.

## Software Setup (Arduino IDE)

1. Open Arduino IDE.
2. Navigate into the `shrike_lite` or `shrike_fi` folder depending on your board.
3. Open `laser_tripwire.ino`.
4. Select the correct board (`ESP32S3 Dev Module` or `Raspberry Pi Pico`).
5. Compile and upload!

## How It Works (State Machine)

1. **Arming Phase:** When the board boots, the laser turns on. You have 3 seconds to align the laser beam directly onto the LDR. The buzzer will beep to indicate it's calibrating.
2. **Calibration:** The system reads the direct laser light intensity and sets the alarm threshold to 75% of that value. The buzzer sounds a high-pitched "Armed" tone.
3. **Armed State:** The system continuously monitors the LDR.
4. **Alarm State:** If someone walks through the beam and blocks the laser, the light drops below the threshold. A dual-tone siren plays continuously. It will not stop even if the beam is restored!
5. **Reset:** Press the Reset button to silence the alarm and re-arm the system.
