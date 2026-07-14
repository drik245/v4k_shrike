# Project 42: LED Bar Graph

Visualize an analog input (like a potentiometer) across 10 LEDs. The higher the voltage, the more LEDs light up in the sequence, creating a bar graph effect. Written in **MicroPython**.

Supports both **Shrike Lite** and **Shrike Fi**.

## Hardware Required
| Component | Qty |
|-----------|-----|
| Shrike Lite or Shrike Fi | 1 |
| LED | 10 |
| 330Ω Resistor | 10 |
| Potentiometer (e.g., 10kΩ) | 1 |

## Wiring / Pinout

### Shrike Lite (RP2040)
| Component | Pin |
|-----------|-----|
| Potentiometer Wiper (Center) | `RP_IO26` (ADC0) |
| LED 1 (Lowest) | `RP_IO16` |
| LED 2 | `RP_IO17` |
| LED 3 | `RP_IO18` |
| LED 4 | `RP_IO19` |
| LED 5 | `RP_IO20` |
| LED 6 | `RP_IO21` |
| LED 7 | `RP_IO22` |
| LED 8 | `RP_IO23` |
| LED 9 | `RP_IO24` |
| LED 10 (Highest) | `RP_IO25` |

### Shrike Fi (ESP32-S3)
| Component | Pin |
|-----------|-----|
| Potentiometer Wiper (Center) | `ESP_IO1` (ADC1_CH0) |
| LED 1 (Lowest) | `ESP_IO15` |
| LED 2 | `ESP_IO16` |
| LED 3 | `ESP_IO17` |
| LED 4 | `ESP_IO18` |
| LED 5 | `ESP_IO33` |
| LED 6 | `ESP_IO34` |
| LED 7 | `ESP_IO35` |
| LED 8 | `ESP_IO36` |
| LED 9 | `ESP_IO37` |
| LED 10 (Highest) | `ESP_IO38` |

**Power connections:** 
- Connect the outer legs of the potentiometer to **3.3V** and **GND**.
- Connect the cathode (shorter leg) of each LED to **GND** through a 330Ω resistor. The anode (longer leg) connects to the specified IO pin.

## Software Setup (MicroPython)

1. Flash your board with MicroPython firmware.
2. Navigate into the `shrike_lite` or `shrike_fi` folder.
3. Upload `main.py` to the board:
   ```bash
   mpremote cp main.py :main.py
   ```
4. Reboot to run:
   ```bash
   mpremote soft-reset
   ```

## How It Works
The script continuously reads the analog value from the potentiometer (0-65535 on RP2040, 0-4095 on ESP32-S3). It maps this value to a number between 0 and 10. It then turns on that number of LEDs in the sequence, updating every 50 milliseconds.
