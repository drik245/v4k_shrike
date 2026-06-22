# ⏰ Alarm Clock

Offline alarm clock on a **TM1637 4-digit seven-segment display** with three push buttons. Timekeeping runs off the ESP32-S3's 40 MHz crystal via `millis()` — no WiFi needed. Set time and alarm manually with buttons.

## Board Variants

| Variant | Board | MCU | Sketch |
|---------|-------|-----|--------|
| [shrike_fi](shrike_fi/) | Shrike Fi | ESP32-S3 | `shrike_fi/alarm_clock.ino` |

## Features

- Offline timekeeping using `millis()` (no WiFi)
- 3-button interface: MODE, UP, DOWN
- Set time and alarm through a mode-cycle menu
- Blinking digits show which value is being edited
- Auto-repeat when holding UP or DOWN for fast value changes
- Buzzer alarm with beep-beep pattern
- Auto-dismiss after 60 seconds, or press any button to stop
- UP toggles alarm on/off in clock mode (briefly shows alarm time)

## Controls

| Button | In Clock Mode | In Set Mode |
|--------|--------------|-------------|
| MODE | Enter set mode | Cycle: Set Hour → Set Min → Set Alarm Hr → Set Alarm Min → Clock |
| UP | Toggle alarm on/off | Increment value (hold to auto-repeat) |
| DOWN | — | Decrement value (hold to auto-repeat) |
| Any button | Dismiss ringing alarm | — |

## Wiring

### Shrike Fi (ESP32-S3)

| Function | Signal | GPIO |
|----------|--------|------|
| TM1637 CLK | ESP_IO1 | 1 |
| TM1637 DIO | ESP_IO2 | 2 |
| MODE button | ESP_IO3 | 3 (active low, internal pullup) |
| UP button | ESP_IO4 | 4 (active low, internal pullup) |
| DOWN button | ESP_IO5 | 5 (active low, internal pullup) |
| Buzzer | ESP_IO6 | 6 |

## Dependencies

Install these via the Arduino Library Manager:

| Library | Author |
|---------|--------|
| **TM1637Display** | Avishay Orpaz |

## Arduino IDE Setup

1. **Board**: `ESP32S3 Dev Module`
2. Open `shrike_fi/alarm_clock.ino` and upload

## Usage

1. On boot, the display shows "SEt" and enters set-hour mode
2. Use UP/DOWN to set the hour, then press MODE
3. Set the minutes, press MODE
4. Set the alarm hour and minute the same way
5. After the last MODE press, the clock starts running with the alarm enabled
6. In clock mode, press UP to toggle the alarm on/off
