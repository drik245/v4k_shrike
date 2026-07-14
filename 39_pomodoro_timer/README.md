# Project 39: Pomodoro Productivity Timer

An offline Pomodoro timer that counts down 25 minutes of work, then switches to a 5-minute rest. The countdown shows on a TM1637 4-digit display with a blinking colon, and an active buzzer fires 3 times when each session ends. Written in **MicroPython**.

Supports both **Shrike Lite** and **Shrike Fi**.

## Hardware Required
| Component | Qty |
|-----------|-----|
| Shrike Lite or Shrike Fi | 1 |
| TM1637 4-digit display | 1 |
| Active buzzer | 1 |
| Momentary push button | 2 |

## Wiring / Pinout

| Component | Shrike Lite (RP2040) | Shrike Fi (ESP32-S3) |
|-----------|----------------------|----------------------|
| TM1637 CLK | `RP_IO6` | `ESP_IO4` |
| TM1637 DIO | `RP_IO7` | `ESP_IO5` |
| Buzzer (+) | `RP_IO10` | `ESP_IO6` |
| Start/Pause Button | `RP_IO14` → GND | `ESP_IO14` → GND |
| Reset Button | `RP_IO15` → GND | `ESP_IO7` → GND |

TM1637 VCC → 3.3V, GND → GND. Buzzer (−) → GND.
Buttons use `INPUT_PULLUP` so no external resistors are needed.

## Software Setup (MicroPython)

Each board folder contains two files: `main.py` and `tm1637.py`. You need to upload both to the board.

1. Flash your board with MicroPython firmware.
2. Navigate into the `shrike_fi` or `shrike_lite` folder.
3. Upload both files:
   ```bash
   mpremote cp tm1637.py :tm1637.py
   mpremote cp main.py :main.py
   ```
4. Reboot to run:
   ```bash
   mpremote soft-reset
   ```

## How to Use
- **Start/Pause:** Press once to start the 25-minute work session. Press again to pause.
- **Auto-Rest:** When work time hits 00:00, the buzzer beeps 3 times and the 5-minute rest timer starts automatically.
- **Auto-Idle:** When rest ends, the buzzer beeps again and the display returns to 25:00.
- **Reset:** Press the reset button at any time to abort and return to 25:00 idle.

## Display Behaviour
| State | Colon |
|-------|-------|
| Idle | Always on |
| Work / Rest (running) | Blinks every second |
| Paused | Always on |
