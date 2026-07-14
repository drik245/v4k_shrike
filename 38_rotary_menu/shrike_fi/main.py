"""
Project 38: Rotary Encoder Menu System (Shrike Fi - ESP32-S3)

A scrollable OLED menu driven by a rotary encoder.
Turn the knob to move the cursor, push the knob to select an item.

Wiring:
  OLED VCC  -> 3.3V
  OLED GND  -> GND
  OLED SCL  -> ESP_IO36  (SPI2_CLK)
  OLED SDA  -> ESP_IO35  (SPI2_MOSI)
  OLED CS   -> ESP_IO34  (SPI2_CS0)
  OLED DC   -> ESP_IO4
  OLED RES  -> ESP_IO5

  Encoder CLK (A) -> ESP_IO1
  Encoder DT  (B) -> ESP_IO2
  Encoder SW      -> ESP_IO3
  Encoder VCC     -> 3.3V
  Encoder GND     -> GND

Library: ssd1306 (built into MicroPython firmware)
"""

from machine import Pin, SPI
from ssd1306 import SSD1306_SPI
import time

# Hardware SPI2 — miso=Pin(37) is ESP_IO37 (SPI2_MISO) on the right header
spi = SPI(2, baudrate=10_000_000, sck=Pin(36), mosi=Pin(35), miso=Pin(37))
oled = SSD1306_SPI(128, 64, spi, dc=Pin(4), res=Pin(5), cs=Pin(34))

# Encoder pins
clk = Pin(1, Pin.IN, Pin.PULL_UP)
dt  = Pin(2, Pin.IN, Pin.PULL_UP)
sw  = Pin(3, Pin.IN, Pin.PULL_UP)

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
    oled.fill(0)
    if item == "LED Blink":
        oled.text("LED Blink!", 20, 20)
        oled.text("(no onboard LED)", 0, 40)
    elif item == "Show Info":
        oled.text("Shrike Fi", 30, 10)
        oled.text("ESP32-S3", 30, 25)
        oled.text("MicroPython", 20, 40)
    elif item == "Counter":
        for n in range(5, 0, -1):
            oled.fill(0)
            oled.text("Counting...", 10, 10)
            oled.text(str(n), 55, 32)
            oled.show()
            time.sleep(1)
    elif item == "About":
        oled.text("Project 38", 20, 10)
        oled.text("Rotary Menu", 16, 28)
        oled.text("Vicharak 2025", 8, 46)
    oled.show()
    time.sleep(2)


draw_menu()

while True:
    new_clk = clk.value()
    if new_clk != last_clk:
        if dt.value() != new_clk:
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
