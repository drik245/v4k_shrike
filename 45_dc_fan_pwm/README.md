# Project 45: DC Fan Speed Control

Control the speed of a DC Fan using a potentiometer. The microcontroller reads the analog voltage from the potentiometer and uses Hardware PWM (Pulse Width Modulation) to control a MOSFET, which in turn regulates the power to the fan. Written in **MicroPython**.

Supports both **Shrike Lite** and **Shrike Fi**.

## Hardware Required
| Component | Qty |
|-----------|-----|
| Shrike Lite or Shrike Fi | 1 |
| DC Fan (e.g., 5V or 12V PC fan) | 1 |
| N-Channel MOSFET (e.g., IRLZ44N) or Motor Driver | 1 |
| Potentiometer (e.g., 10kΩ) | 1 |
| External Power Supply (if fan needs >3.3V) | 1 |

## Wiring / Pinout

### Shrike Lite (RP2040)
| Component | Pin |
|-----------|-----|
| Potentiometer Wiper (Center) | `RP_IO26` (ADC0) |
| MOSFET Gate (PWM Signal) | `RP_IO14` |

### Shrike Fi (ESP32-S3)
| Component | Pin |
|-----------|-----|
| Potentiometer Wiper (Center) | `ESP_IO1` (ADC_CH0) |
| MOSFET Gate (PWM Signal) | `ESP_IO14` |

**Power connections:** 
- **Potentiometer**: Connect the outer legs to **3.3V** and **GND** on the Shrike board.
- **MOSFET**: 
  - Connect the **Source** pin to GND.
  - Connect the **Drain** pin to the Negative (-) wire of the DC fan.
  - Connect the **Gate** pin to the PWM Signal pin on the Shrike board.
- **Fan Power**: Connect the Positive (+) wire of the DC fan directly to the external power supply (e.g., 5V or 12V). **Make sure the ground of the external supply is tied to the GND of the Shrike board!**

## Software Setup (MicroPython)

1. Flash your board with MicroPython firmware.
2. Navigate into the `shrike_lite` or `shrike_fi` folder depending on your board.
3. Upload `main.py` to the board:
   ```bash
   mpremote cp main.py :main.py
   ```
4. Reboot to run:
   ```bash
   mpremote soft-reset
   ```

## How It Works
The ADC (Analog-to-Digital Converter) reads the position of the potentiometer. This reading is mapped to a 16-bit value (0-65535) and passed to the hardware PWM controller running at 1000Hz. The PWM signal rapidly turns the MOSFET on and off, effectively varying the average voltage (and therefore the speed) delivered to the fan.
