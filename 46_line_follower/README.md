# Project 46: Line Follower Robot

A classic 2-wheel line follower robot! Using two infrared (IR) sensors facing the ground, the robot stays on a black line drawn on a white surface. An L298N motor driver controls the two DC motors. Written in **MicroPython**.

Supports both **Shrike Lite** and **Shrike Fi**.

## Hardware Required
| Component | Qty |
|-----------|-----|
| Shrike Lite or Shrike Fi | 1 |
| IR Obstacle / Line Tracking Sensor | 2 |
| L298N Motor Driver Module | 1 |
| DC Gear Motor (with wheels) | 2 |
| Robot Chassis | 1 |
| Battery Pack (e.g. 2x 18650 or 4x AA) | 1 |

## Wiring / Pinout

### Shrike Lite (RP2040)
| Component | Pin |
|-----------|-----|
| Left IR Sensor (OUT) | `RP_IO14` |
| Right IR Sensor (OUT)| `RP_IO15` |
| L298N IN1 (Motor A Fwd) | `RP_IO5` |
| L298N IN2 (Motor A Rev) | `RP_IO6` |
| L298N IN3 (Motor B Fwd) | `RP_IO7` |
| L298N IN4 (Motor B Rev) | `RP_IO8` |

### Shrike Fi (ESP32-S3)
| Component | Pin |
|-----------|-----|
| Left IR Sensor (OUT) | `ESP_IO14` |
| Right IR Sensor (OUT)| `ESP_IO7` |
| L298N IN1 (Motor A Fwd) | `ESP_IO4` |
| L298N IN2 (Motor A Rev) | `ESP_IO5` |
| L298N IN3 (Motor B Fwd) | `ESP_IO6` |
| L298N IN4 (Motor B Rev) | `ESP_IO15` |

**Power connections:** 
- Connect the **VCC** of both IR sensors to **3.3V** or **5V** on the Shrike board.
- Connect the battery pack positive wire to the **12V / VMOT** input on the L298N.
- Connect the **5V output** of the L298N to the **5V / VBUS** pin of the Shrike board to power it.
- **CRITICAL:** Tie all **GND** pins together! (Battery GND, L298N GND, Shrike GND, IR Sensor GNDs).

## Software Setup (MicroPython)

1. Flash your board with MicroPython firmware.
2. Navigate into the `shrike_lite` or `shrike_fi` folder.
3. Upload `main.py` to the board:
   ```bash
   mpremote cp main.py :main.py
   ```
4. Place the robot on a track (black electrical tape on a white floor works great) and reboot:
   ```bash
   mpremote soft-reset
   ```

## How It Works
- The IR sensors shoot infrared light downwards. White surfaces reflect the light back (logic 0), while black lines absorb it (logic 1).
- **Both see white:** The robot is centered over the line (straddling it), so it moves forward.
- **Left sees black:** The robot drifted right. It stops the left wheel to turn left and re-center.
- **Right sees black:** The robot drifted left. It stops the right wheel to turn right and re-center.
- **Both see black:** The robot hit a horizontal stop line (T-junction), so it stops.
