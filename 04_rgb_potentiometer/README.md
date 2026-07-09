# Project 04: RGB LED Potentiometer Controller

This project uses MicroPython to smoothly control a Common Anode RGB LED using three independent potentiometers, one for each color channel (Red, Green, Blue). 

## 1. Hardware & Wiring

- **Outer Pins:** Connect all outer left pins to `3.3V` and outer right pins to `GND`.
- **Red Pot Middle Pin:** `RP_IO26` (Lite) or `ESP_IO1` (Fi)
- **Green Pot Middle Pin:** `RP_IO27` (Lite) or `ESP_IO2` (Fi)
- **Blue Pot Middle Pin:** `RP_IO28` (Lite) or `ESP_IO3` (Fi)

### RGB LED (Common Anode)
- **Common Pin (Longest):** `3.3V`
- **Red Pin:** `RP_IO10` (Lite) or `ESP_IO4` (Fi)
- **Green Pin:** `RP_IO11` (Lite) or `ESP_IO5` (Fi)
- **Blue Pin:** `RP_IO14` (Lite) or `ESP_IO6` (Fi)

## 2. Software Setup

### How It Works
Because you are using a **Common Anode** LED, the PWM logic is inverted. Sending a high voltage (100% duty cycle) to the pin means there is no voltage difference between the Anode (3.3V) and the pin, so the LED turns OFF. Sending a low voltage (0% duty cycle) creates a voltage drop, turning the LED ON at maximum brightness. The `main.py` code handles this math automatically while simultaneously smoothing out ADC electrical noise using an Exponential Moving Average (EMA) filter.

### Flashing the Code
1. Open your terminal in the `04_rgb_potentiometer` directory.
2. Upload the script to your board:
   ```bash
   python -m mpremote cp main.py :main.py
   ```
3. Reset the board (or run `python -m mpremote reset`).
4. Turn the knobs and watch the colors mix!
