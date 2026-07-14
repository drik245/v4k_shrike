# Project 48: Automated Plant Watering

Never forget to water your plants again! This project reads the moisture level of the soil using an analog sensor. When the soil becomes too dry, it automatically triggers a relay module to turn on a 5V submersible water pump. Written in **MicroPython**.

Supports both **Shrike Lite** and **Shrike Fi**.

## Hardware Required
| Component | Qty |
|-----------|-----|
| Shrike Lite or Shrike Fi | 1 |
| Analog Soil Moisture Sensor (with LM393 comparator board) | 1 |
| 5V Relay Module (1-Channel) | 1 |
| 5V Mini Submersible Water Pump | 1 |
| Silicone Tubing | 1 |
| External 5V Power Supply | 1 |

## Wiring / Pinout

### Shrike Lite (RP2040)
| Component | Pin |
|-----------|-----|
| Soil Sensor Analog Out (A0) | `RP_IO26` (ADC0) |
| Relay Module IN / Signal | `RP_IO14` |

### Shrike Fi (ESP32-S3)
| Component | Pin |
|-----------|-----|
| Soil Sensor Analog Out (A0) | `ESP_IO1` (ADC_CH0) |
| Relay Module IN / Signal | `ESP_IO14` |

**Power connections:** 
- **Soil Sensor**: Connect VCC to **3.3V** on the Shrike board. Connect GND to GND.
- **Relay Module**: Connect VCC to **5V** (the relay coil needs 5V to switch securely). Connect GND to GND.
- **Water Pump**: 
  - Connect the Pump's Negative (-) wire to the External 5V Power Supply GND.
  - Connect the Pump's Positive (+) wire to the **Normally Open (NO)** terminal of the relay.
  - Connect the **Common (COM)** terminal of the relay to the External 5V Power Supply 5V.
- **CRITICAL:** Make sure the GND of your external 5V power supply is tied to the GND of the Shrike board.

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
The ADC (Analog-to-Digital Converter) continuously reads the voltage from the soil sensor. 
- **Wet Soil:** Water conducts electricity, lowering the resistance between the sensor prongs, resulting in a low voltage reading.
- **Dry Soil:** Air does not conduct, increasing resistance, resulting in a high voltage reading.
If the reading exceeds the `DRY_THRESHOLD` defined in the code, it sends a `HIGH` signal to the relay, closing the circuit and turning on the water pump until the moisture level is restored.
