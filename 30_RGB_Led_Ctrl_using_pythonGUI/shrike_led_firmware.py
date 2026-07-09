# runs on the board, controls the rgb led over pwm
# talks to the pc over usb serial, just plain text commands

from machine import Pin, PWM
import micropython
import sys
import time
import math
import random
import select

micropython.kbd_intr(-1)

# change this depending on which board you're flashing
BOARD = "shrike_lite"   # "shrike_lite" or "shrike_fi"

# gpio pins for each board, picked to avoid the fpga bridge pins and stuff
PINS = {
    "shrike_lite": {"R": 10, "G": 11, "B": 14},
    "shrike_fi":   {"R": 4,  "G": 5,  "B": 6},
}

PWM_FREQ = 1000

# my led is common-anode, common pin goes to 3v3 not ground
# so higher pwm duty = dimmer, need to flip the numbers
COMMON_ANODE = True

BREATH_PERIOD_S = 3.0     # how long one breathe in-out takes
DISCO_INTERVAL_MS = 150   # how fast disco flashes change
CYCLE_PERIOD_S = 5.0      # how long a full colour cycle takes

pins = PINS[BOARD]
led = {}
for ch, gpio in pins.items():
    p = PWM(Pin(gpio))
    p.freq(PWM_FREQ)
    p.duty_u16(65535 if COMMON_ANODE else 0)  # off at boot
    led[ch] = p


def set_channel(ch, value_8bit):
    value_8bit = max(0, min(255, int(value_8bit)))
    duty = value_8bit * 257  # 0-255 -> 0-65535
    if COMMON_ANODE:
        duty = 65535 - duty
    led[ch].duty_u16(duty)


def set_rgb(r, g, b):
    set_channel("R", r)
    set_channel("G", g)
    set_channel("B", b)


def all_off():
    set_rgb(0, 0, 0)


mode = "manual"          # manual, breath, or disco
last_rgb = [0, 0, 0]     # last colour someone actually picked, breath fades around this


def handle_line(line):
    global mode
    try:
        if line == "OFF":
            mode = "manual"
            all_off()
            last_rgb[0] = last_rgb[1] = last_rgb[2] = 0
        elif line == "MODE:BREATH":
            mode = "breath"
        elif line == "MODE:DISCO":
            mode = "disco"
        elif line == "MODE:CYCLE":
            mode = "cycle"
        elif line == "MODE:MANUAL":
            mode = "manual"
        elif line.startswith("RGB:"):
            mode = "manual"
            r, g, b = (int(x) for x in line[4:].split(","))
            set_rgb(r, g, b)
            last_rgb[0], last_rgb[1], last_rgb[2] = r, g, b
        elif ":" in line:
            mode = "manual"
            ch_name, val = line.split(":")
            ch_name = ch_name.strip().upper()
            if ch_name in led:
                v = max(0, min(255, int(val.strip())))
                set_channel(ch_name, v)
                idx = {"R": 0, "G": 1, "B": 2}[ch_name]
                last_rgb[idx] = v
            else:
                raise ValueError("unknown channel " + ch_name)
        else:
            raise ValueError("bad command")
        print("OK")
    except Exception as e:
        print("ERR", e)


def breath_step(t):
    # sine wave between 0 and 1 for brightness
    level = (math.sin(2 * math.pi * t / BREATH_PERIOD_S) + 1) / 2
    r, g, b = last_rgb
    if r == 0 and g == 0 and b == 0:
        r = g = b = 255  # nothing was picked yet, just breathe white
    set_rgb(int(r * level), int(g * level), int(b * level))


def disco_step():
    set_rgb(random.randint(0, 255), random.randint(0, 255), random.randint(0, 255))


def cycle_step(t):
    # sine waves offset by 120 degrees for R, G, B to sweep through all colours
    freq = 2 * math.pi / CYCLE_PERIOD_S
    r = (math.sin(freq * t) + 1) * 127.5
    g = (math.sin(freq * t + 2 * math.pi / 3) + 1) * 127.5
    b = (math.sin(freq * t + 4 * math.pi / 3) + 1) * 127.5
    set_rgb(int(r), int(g), int(b))


all_off()
print("READY", BOARD)

t0 = time.ticks_ms()
last_disco = time.ticks_ms()

while True:
    # everything in here is wrapped - a bad/partial read should never be
    # able to kill this loop and strand the board silently at the REPL
    try:
        r, _, _ = select.select([sys.stdin], [], [], 0)
        if r:
            line = sys.stdin.readline().strip()
            if line:
                handle_line(line)

        if mode == "breath":
            t = time.ticks_diff(time.ticks_ms(), t0) / 1000
            breath_step(t)
        elif mode == "cycle":
            t = time.ticks_diff(time.ticks_ms(), t0) / 1000
            cycle_step(t)
        elif mode == "disco":
            now = time.ticks_ms()
            if time.ticks_diff(now, last_disco) >= DISCO_INTERVAL_MS:
                disco_step()
                last_disco = now
    except Exception as e:
        print("ERR loop", e)

    time.sleep_ms(15)