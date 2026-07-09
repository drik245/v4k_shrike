# Project 30: RGB LED Control using Python GUI

A full-stack project combining MicroPython on the Shrike board with a custom graphical user interface (GUI) on your PC!

It allows you to control an RGB LED in real-time over a USB serial connection. The GUI features a color picker, individual R/G/B sliders, a Hue slider, and built-in animation modes (Breath, Disco, and Cycle).

## 🛠️ The Architecture

This project is split into two halves:
1. **The Firmware (`shrike_led_firmware.py`)**: A MicroPython script that runs on the Shrike board. It listens to the USB Serial port for text commands (e.g., `RGB:255,0,0`, `MODE:BREATH`) and translates them into high-frequency PWM signals for the LED. It specifically disables `Ctrl-C` (`micropython.kbd_intr(-1)`) to ensure the USB serial communication isn't accidentally interrupted by stray data.
2. **The GUI (`shrike_led_gui.py`)**: A Python desktop app that runs on your computer. It uses a background thread to seamlessly connect to the board's COM port without freezing the UI, and sends commands whenever you move a slider or click a button.

## Hardware Wiring

By default, the code is configured for a **Common Anode** RGB LED (the longest leg connects to 3.3V). If you have a Common Cathode LED (longest leg connects to GND), change `COMMON_ANODE = False` in the firmware.

### Shrike Lite (RP2040)
| RGB LED Pin | Shrike Lite Pin |
|-------------|-----------------|
| Red | RP_IO10 |
| Green | RP_IO11 |
| Blue | RP_IO14 |
| Common (Longest) | 3V3 (or GND if Common Cathode) |

### Shrike Fi (ESP32-S3)
| RGB LED Pin | Shrike Fi Pin |
|-------------|---------------|
| Red | ESP_IO4 |
| Green | ESP_IO5 |
| Blue | ESP_IO6 |
| Common (Longest) | 3V3 (or GND if Common Cathode) |

> Ensure you use current-limiting resistors (e.g., 220Ω) on the R, G, and B lines to protect your board and LED!

## 🚀 Setup & Usage

### 1. Flash the Board
1. Upload `shrike_led_firmware.py` to your board but save it as `main.py`.
   ```bash
   python -m mpremote cp shrike_led_firmware.py :main.py
   ```

### 2. Run the PC GUI
On your computer, you need Python installed. Install the required libraries via terminal/command prompt:
```bash
pip install customtkinter pyserial CTkColorPicker
```

Then, run the GUI script:
```bash
python shrike_led_gui.py
```

### 3. Connect and Play
1. In the GUI, select your board's COM port from the dropdown.
2. Click **Connect**.
3. Use the sliders, color picker, or effect buttons to control the LED! 
4. The status text at the bottom will show you exactly what the board is doing or report any errors.
