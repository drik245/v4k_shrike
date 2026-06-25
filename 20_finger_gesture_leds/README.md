# 🖐️ Finger Gesture Controlled LEDs

This project uses a PC webcam to track your hand using OpenCV and MediaPipe. It counts how many fingers you are holding up and sends a serial command to your Shrike board, which then instantly turns on the corresponding number of LEDs (1 to 5).

> [!WARNING]
> **This project uses MicroPython!** Unlike the previous projects in this repository that use Arduino C++, you must flash the MicroPython firmware onto your Shrike board for this script to work.

## Board Variants

| Variant | Board | MCU | Script |
|---------|-------|-----|--------|
| [shrike_fi](shrike_fi/) | Shrike Fi | ESP32-S3 | `shrike_fi/main.py` |
| [shrike_lite](shrike_lite/) | Shrike Lite | RP2040 | `shrike_lite/main.py` |

## Hardware Setup

Connect 5 LEDs to your board via 220Ω resistors.

### Shrike Fi Wiring
| LED | GPIO |
|-----|------|
| LED 1 | ESP_IO1 |
| LED 2 | ESP_IO2 |
| LED 3 | ESP_IO3 |
| LED 4 | ESP_IO4 |
| LED 5 | ESP_IO5 |

### Shrike Lite Wiring
| LED | GPIO |
|-----|------|
| LED 1 | RP_IO26 |
| LED 2 | RP_IO27 |
| LED 3 | RP_IO28 |
| LED 4 | RP_IO29 |
| LED 5 | RP_IO15 |

## PC Setup

You need Python installed on your PC with a webcam connected.

1. Open a terminal and install the dependencies:
```bash
pip install numpy==1.26.4 opencv-python mediapipe==0.10.14 cvzone pyserial
```

2. Open `pc_script/finger_led.py` in a text editor and **change `COM_PORT = 'COM14'` to match your Shrike board's COM port**.

## Running the Project

1. Flash the MicroPython firmware to your Shrike board.
2. Upload `main.py` to the board using a tool like Thonny or mpremote.
3. Keep the board plugged in via USB (this acts as the serial bridge).
4. Run the python script on your PC:
```bash
python pc_script/finger_led.py
```
5. Hold your hand up to the webcam. As you raise fingers, the LEDs on your breadboard will light up in real time!
