"""
MicroPython TM1637 quad 7-segment LED display driver
https://github.com/mcauser/micropython-tm1637

MIT License
Copyright (c) 2016-2023 Mike Causer
"""

__version__ = '1.3.0'

from micropython import const
from machine import Pin
from time import sleep_us, sleep_ms

TM1637_CMD1 = const(64)   # 0x40 data command
TM1637_CMD2 = const(192)  # 0xC0 address command
TM1637_CMD3 = const(128)  # 0x80 display control command
TM1637_DSP_ON = const(8)  # 0x08 display on
TM1637_DELAY = const(10)  # 10us delay between clk/dio pulses
TM1637_MSB = const(128)   # msb is the decimal point or colon

_SEGMENTS = bytearray(b'\x3F\x06\x5B\x4F\x66\x6D\x7D\x07\x7F\x6F\x77\x7C\x39\x5E\x79\x71\x3D\x76\x06\x1E\x76\x38\x55\x54\x3F\x73\x67\x50\x6D\x78\x3E\x1C\x2A\x76\x6E\x5B\x00\x40\x63')

class TM1637(object):
    def __init__(self, clk, dio, brightness=7):
        self.clk = clk
        self.dio = dio
        if not 0 <= brightness <= 7:
            raise ValueError("Brightness out of range")
        self._brightness = brightness
        self.clk.init(Pin.OUT, value=0)
        self.dio.init(Pin.OUT, value=0)
        sleep_us(TM1637_DELAY)
        self._write_data_cmd()
        self._write_dsp_ctrl()

    def _start(self):
        self.dio(0); sleep_us(TM1637_DELAY)
        self.clk(0); sleep_us(TM1637_DELAY)

    def _stop(self):
        self.dio(0); sleep_us(TM1637_DELAY)
        self.clk(1); sleep_us(TM1637_DELAY)
        self.dio(1)

    def _write_data_cmd(self):
        self._start()
        self._write_byte(TM1637_CMD1)
        self._stop()

    def _write_dsp_ctrl(self):
        self._start()
        self._write_byte(TM1637_CMD3 | TM1637_DSP_ON | self._brightness)
        self._stop()

    def _write_byte(self, b):
        for i in range(8):
            self.clk(0); sleep_us(TM1637_DELAY)
            self.dio((b >> i) & 1); sleep_us(TM1637_DELAY)
            self.clk(1); sleep_us(TM1637_DELAY)
        self.clk(0); sleep_us(TM1637_DELAY)
        self.clk(1); sleep_us(TM1637_DELAY)
        self.clk(0); sleep_us(TM1637_DELAY)

    def brightness(self, val=None):
        if val is None:
            return self._brightness
        if not 0 <= val <= 7:
            raise ValueError("Brightness out of range")
        self._brightness = val
        self._write_data_cmd()
        self._write_dsp_ctrl()

    def write(self, segments, pos=0):
        if not 0 <= pos <= 5:
            raise ValueError("Position out of range")
        self._write_data_cmd()
        self._start()
        self._write_byte(TM1637_CMD2 | pos)
        for seg in segments:
            self._write_byte(seg)
        self._stop()
        self._write_dsp_ctrl()

    def encode_digit(self, digit):
        return _SEGMENTS[digit & 0x0f]

    def encode_string(self, s):
        segments = bytearray(len(s))
        for i, c in enumerate(s):
            if c == ' ':
                pass
            elif c == '-':
                segments[i] = _SEGMENTS[36]
            elif c >= '0' and c <= '9':
                segments[i] = _SEGMENTS[ord(c) - ord('0')]
            elif c >= 'a' and c <= 'z':
                segments[i] = _SEGMENTS[ord(c) - ord('a') + 10]
            elif c >= 'A' and c <= 'Z':
                segments[i] = _SEGMENTS[ord(c) - ord('A') + 10]
        return segments

    def encode_char(self, c):
        return self.encode_string(c)[0]

    def hex(self, val):
        string = '{:04x}'.format(val & 0xffff)
        self.write(self.encode_string(string))

    def number(self, num):
        num = max(-999, min(num, 9999))
        string = '{:4d}'.format(num)
        self.write(self.encode_string(string))

    def numbers(self, num1, num2, colon=True):
        segments = self.encode_string('{0:0>2}{1:0>2}'.format(
            min(num1, 99), min(num2, 99)))
        if colon:
            segments[1] |= 0x80
        self.write(segments)

    def temperature(self, num):
        if num < -9:
            self.show('--dc')
        elif num > 99:
            self.show('--dc')
        else:
            string = '{:2d}dc'.format(num)
            self.write(self.encode_string(string))

    def show(self, string, colon=False):
        segments = self.encode_string(string)
        if colon:
            segments[1] |= 0x80
        self.write(segments[:4])

    def scroll(self, string, delay=250):
        segments = string if isinstance(string, list) else self.encode_string(string)
        data = [0] * 8 + list(segments) + [0] * 8
        for i in range(len(data) - 3):
            self.write(data[i:i+4])
            sleep_ms(delay)

    def off(self):
        self.write([0, 0, 0, 0])
