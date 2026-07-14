"""
Project 39: Pomodoro Productivity Timer (Shrike Lite - RP2040)

25-minute work session, then 5-minute rest. Countdown on TM1637.
Colon blinks every second while running. Buzzer fires 3 times when
a session ends and the next one starts automatically.

Requires tm1637.py in the same folder on your board:
  mpremote cp tm1637.py :tm1637.py
  mpremote cp main.py :main.py

Wiring:
  TM1637 CLK  -> RP_IO6
  TM1637 DIO  -> RP_IO7
  Buzzer (+)  -> RP_IO10  (active buzzer, other leg to GND)
  Start/Pause -> RP_IO14 to GND
  Reset       -> RP_IO15 to GND

  TM1637 VCC -> 3.3V, GND -> GND
  Buttons use INPUT_PULLUP, no resistors needed.
"""

from machine import Pin, PWM
import tm1637
import time

display   = tm1637.TM1637(clk=Pin(6), dio=Pin(7))
buzzer    = PWM(Pin(10), freq=1000, duty_u16=0)
btn_start = Pin(14, Pin.IN, Pin.PULL_UP)
btn_reset = Pin(15, Pin.IN, Pin.PULL_UP)

WORK_TIME = 25 * 60
REST_TIME = 5 * 60
DEBOUNCE  = 200  # ms

IDLE   = 0
WORK   = 1
REST   = 2
PAUSED = 3

state     = IDLE
remaining = WORK_TIME
colon_on  = True

last_tick        = time.ticks_ms()
last_start_press = 0
last_reset_press = 0


def beep(count=3):
    for _ in range(count):
        buzzer.duty_u16(32768)
        time.sleep_ms(200)
        buzzer.duty_u16(0)
        time.sleep_ms(100)


def refresh():
    m = remaining // 60
    s = remaining % 60
    display.numbers(m, s, colon=colon_on)


refresh()

while True:
    now = time.ticks_ms()

    # Start/Pause button
    if btn_start.value() == 0 and time.ticks_diff(now, last_start_press) > DEBOUNCE:
        last_start_press = now
        if state in (IDLE, PAUSED):
            state = WORK
        elif state in (WORK, REST):
            state = PAUSED

    # Reset button
    if btn_reset.value() == 0 and time.ticks_diff(now, last_reset_press) > DEBOUNCE:
        last_reset_press = now
        state = IDLE
        remaining = WORK_TIME
        colon_on = True
        refresh()

    # Countdown — tick every 500ms, decrement every full second (2 ticks)
    if state in (WORK, REST):
        if time.ticks_diff(now, last_tick) >= 500:
            last_tick = now
            colon_on = not colon_on
            if colon_on:  # decrement once per second
                remaining -= 1

            if remaining <= 0:
                beep(3)
                if state == WORK:
                    state = REST
                    remaining = REST_TIME
                else:
                    state = IDLE
                    remaining = WORK_TIME

            refresh()

    time.sleep_ms(20)
