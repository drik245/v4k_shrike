"""
Project 38: Rotary Encoder Menu System (Shrike Lite - RP2040)

A scrollable OLED menu driven by a rotary encoder.
Turn the knob to move the cursor, push the knob to select an item.

Wiring:
  OLED VCC  -> 3.3V
  OLED GND  -> GND
  OLED SCL  -> RP_IO10
  OLED SDA  -> RP_IO11
  OLED CS   -> RP_IO14
  OLED DC   -> RP_IO15
  OLED RES  -> RP_IO9
  Dummy MISO-> RP_IO8

  Encoder CLK (A) -> RP_IO16
  Encoder DT  (B) -> RP_IO17
  Encoder SW      -> RP_IO26
  Encoder Power   -> KY-040 VCC to 3.3V, GND to GND. (If using a raw bare encoder, ignore VCC and just wire the Common pins to GND).

Library: ssd1306 (built into MicroPython firmware)
"""

from machine import Pin, SPI
from ssd1306 import SSD1306_SPI
import time

# Hardware SPI1 — matching your previous projects!
spi = SPI(1, baudrate=10_000_000, sck=Pin(10), mosi=Pin(11), miso=Pin(8))
oled_cs = Pin(14, Pin.OUT)
oled_dc = Pin(15, Pin.OUT)
oled_res = Pin(9, Pin.OUT)
oled = SSD1306_SPI(128, 64, spi, oled_dc, oled_res, oled_cs)

# Encoder pins (moved to avoid OLED conflict)
clk = Pin(16, Pin.IN, Pin.PULL_UP)
dt  = Pin(17, Pin.IN, Pin.PULL_UP)
sw  = Pin(26, Pin.IN, Pin.PULL_UP)

MENU_ITEMS = ["LED Blink", "Show Info", "Counter", "About"]
cursor = 0
scroll_offset = 0
VISIBLE_ROWS = 3
LINE_H = 18

last_clk = clk.value()
last_sw  = sw.value()


def draw_menu():
    oled.fill(0)
    oled.text("== SHRIKE MENU ==", 0, 0)
    for i in range(VISIBLE_ROWS):
        idx = i + scroll_offset
        if idx >= len(MENU_ITEMS):
            break
        prefix = ">" if idx == cursor else " "
        oled.text(f"{prefix} {MENU_ITEMS[idx]}", 0, 14 + i * LINE_H)
    bar_h = max(4, 50 // len(MENU_ITEMS))
    bar_y = 14 + int((cursor / len(MENU_ITEMS)) * 50)
    oled.fill_rect(124, bar_y, 4, bar_h, 1)
    oled.show()


def run_action(item):
    # Wait for the user to release the button first!
    while sw.value() == 0:
        time.sleep_ms(10)

    oled.fill(0)
    if item == "LED Blink":
        oled.text("LED Blink!", 20, 20)
        oled.text("Press to exit", 10, 45)
        oled.show()
        led = Pin(25, Pin.OUT)
        state = 1
        while sw.value() == 1:
            led.value(state)
            state = 1 - state
            for _ in range(20):  # 200ms non-blocking delay
                if sw.value() == 0: break
                time.sleep_ms(10)
        led.value(0)
        return
    elif item == "Show Info":
        oled.text("Shrike Lite", 20, 10)
        oled.text("RP2040", 40, 25)
        oled.text("MicroPython", 20, 40)
        oled.text("Press to exit", 10, 55)
        oled.show()
        while sw.value() == 1:
            time.sleep_ms(10)
        return
    elif item == "Counter":
        count = 1
        while sw.value() == 1:
            oled.fill(0)
            oled.text("Counting up:", 20, 10)
            oled.text(str(count), 55, 32)
            oled.text("Press to exit", 10, 50)
            oled.show()
            count += 1
            for _ in range(100): # 1000ms non-blocking delay
                if sw.value() == 0: break
                time.sleep_ms(10)
        return
    elif item == "About":
        credits = ["Project 38", "Rotary Menu", "", "Created by:", "@drik245", "", "Vicharak 2026", "Thank You!"]
        y_offset = 47
        while sw.value() == 1:
            oled.fill(0)
            
            # Draw credits
            for i, line in enumerate(credits):
                text_y = int(y_offset) + i * 12
                if -10 < text_y < 70:
                    x = (128 - len(line) * 8) // 2
                    oled.text(line, x, text_y)
            
            # Draw solid black masks over the top and bottom to create a strict 3-line window
            oled.fill_rect(0, 0, 128, 17, 0)
            oled.fill_rect(0, 47, 128, 17, 0)
            
            oled.show()
            y_offset -= 0.5  # Smooth 0.5 pixel scrolling speed
            
            if y_offset < -(len(credits) * 12):
                y_offset = 47  # Reset to bottom of window
                
            for _ in range(2):
                if sw.value() == 0: break
                time.sleep_ms(10)
        return


draw_menu()

while True:
    new_clk = clk.value()
    # Only trigger on falling edge (1 -> 0) to avoid double-counting detents
    if last_clk == 1 and new_clk == 0:
        if dt.value() == 1:
            cursor = (cursor + 1) % len(MENU_ITEMS)
        else:
            cursor = (cursor - 1) % len(MENU_ITEMS)
        if cursor < scroll_offset:
            scroll_offset = cursor
        elif cursor >= scroll_offset + VISIBLE_ROWS:
            scroll_offset = cursor - VISIBLE_ROWS + 1
        draw_menu()
    last_clk = new_clk

    new_sw = sw.value()
    if new_sw == 0 and last_sw == 1:
        time.sleep_ms(50)
        if sw.value() == 0:
            run_action(MENU_ITEMS[cursor])
            draw_menu()
    last_sw = new_sw
    time.sleep_ms(5)
