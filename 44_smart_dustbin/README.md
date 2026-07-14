# Project 44: Smart Dustbin

Build an auto-opening dustbin! When you wave your hand over the ultrasonic sensor, the servo motor will flip the lid open, hold it for 3 seconds, and then close it automatically. Written in **MicroPython**.

Supports both **Shrike Lite** and **Shrike Fi**.

## Hardware Required
| Component | Qty |
|-----------|-----|
| Shrike Lite or Shrike Fi | 1 |
| HC-SR04 Ultrasonic Sensor | 1 |
| Micro Servo (e.g. SG90) | 1 |

## Wiring / Pinout

### Shrike Lite (RP2040)
| Component | Pin |
|-----------|-----|
| Ultrasonic TRIG | `RP_IO14` |
| Ultrasonic ECHO | `RP_IO15` |
| Servo Signal (PWM) | `RP_IO10` |

### Shrike Fi (ESP32-S3)
| Component | Pin |
|-----------|-----|
| Ultrasonic TRIG | `ESP_IO14` |
| Ultrasonic ECHO | `ESP_IO7` |
| Servo Signal (PWM) | `ESP_IO6` |

**Power connections:** 
- Connect the **VCC** pins of both the HC-SR04 and the Servo to **5V**. (If using a 3.3V-compatible HC-SR04, 3.3V is fine, but servos always need 5V).
- Connect all **GND** pins together.

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
- The code uses `machine.time_pulse_us` to precisely measure the time it takes for the ultrasonic ping to return.
- If the calculated distance is less than 20cm, it triggers the servo.
- The servo is controlled via hardware PWM at 50Hz. The `duty_ns()` function is used to set the exact pulse width in nanoseconds (500,000ns for 0 degrees, 1,500,000ns for 90 degrees) which is much more stable than standard Arduino servo libraries.
