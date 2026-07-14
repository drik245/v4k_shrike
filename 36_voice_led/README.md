# Project 36: Voice-Controlled LED

This project turns your PC into an AI voice assistant that controls hardware! A Python script running on your PC uses your microphone and the `SpeechRecognition` library to listen for commands. When it hears "on" or "off", it sends a command over the USB Serial cable to the Shrike board, which instantly toggles an LED.

This project is fully cross-board compatible. You can flash the exact same code to either the Shrike Lite or Shrike Fi.

## Hardware Required
- Shrike Lite or Shrike Fi
- An LED (with a 220Ω or 330Ω resistor)
- PC with a microphone

## Wiring / Pinout
*The code automatically adapts to whichever board you select in the Arduino IDE!*

| Component | Shrike Lite (RP2040) | Shrike Fi (ESP32-S3) |
|-----------|----------------------|----------------------|
| **LED Positive (Anode)** | `RP_IO14` | `ESP_IO14` |
| **LED Negative (Cathode)** | `GND` via Resistor | `GND` via Resistor |

## Software Setup (Arduino IDE)
1. Navigate into either the `shrike_fi` or `shrike_lite` folder, depending on your board.
2. Open the respective `.ino` file in the Arduino IDE.
3. Select the correct board from the boards menu (`ESP32S3 Dev Module` or `Raspberry Pi Pico`).
4. Compile and upload!
5. *Important: Note down which COM port your board is connected to (e.g., COM3).*

## Software Setup (PC Python Script)
You need Python installed on your PC to run the microphone listener.

1. Open a terminal/command prompt on your PC and install the required Python libraries:
```bash
pip install pyserial SpeechRecognition PyAudio
```
*(Note: If PyAudio fails to install on Windows, you might need to use `pip install pipwin` then `pipwin install pyaudio`)*

2. Open the `voice_controller.py` file in a text editor (or VS Code).
3. Find the line that says `COM_PORT = 'COM3'` and change `'COM3'` to match the actual COM port of your Shrike board.
4. Run the script:
```bash
python voice_controller.py
```

## How to Use
1. Keep the Python script running in your terminal.
2. Say something like, **"Hey computer, turn the light ON."**
3. The script will print what you said, and if it detects the word "on", it will send the command to the board, lighting up your LED instantly!
