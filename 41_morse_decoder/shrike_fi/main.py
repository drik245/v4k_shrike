"""
Project 41: Morse Code Decoder (Shrike Fi - ESP32-S3)

Press the button to enter morse code. The OLED shows the current
dots/dashes as you type, and decodes each letter when you pause.
A longer pause inserts a space between words.

Timing:
  < 50ms      : ignored (noise)
  50-300ms    : dot
  >= 300ms    : dash
  gap > 600ms : end of letter - decode and show
  gap > 1500ms: word space

Wiring:
  OLED SCL  -> ESP_IO36  (SPI2_CLK)
  OLED SDA  -> ESP_IO35  (SPI2_MOSI)
  OLED CS   -> ESP_IO34  (SPI2_CS0)
  OLED DC   -> ESP_IO4
  OLED RES  -> ESP_IO5
  OLED VCC  -> 3.3V, GND -> GND

  Button    -> ESP_IO14 to GND  (INPUT_PULLUP, no resistor needed)
"""

from machine import Pin, SPI
from ssd1306 import SSD1306_SPI
import time

# Hardware SPI2 — miso set explicitly to ESP_IO37 to prevent pin hijacking
spi = SPI(2, baudrate=10_000_000, sck=Pin(36), mosi=Pin(35), miso=Pin(37))
oled = SSD1306_SPI(128, 64, spi, dc=Pin(4), res=Pin(5), cs=Pin(34))

btn = Pin(14, Pin.IN, Pin.PULL_UP)
clear_btn = Pin(11, Pin.IN, Pin.PULL_UP)

MORSE = {
    '.-': 'A',    '-...': 'B',  '-.-.': 'C',  '-..': 'D',   '.': 'E',
    '..-.': 'F',  '--.': 'G',   '....': 'H',  '..': 'I',    '.---': 'J',
    '-.-': 'K',   '.-..': 'L',  '--': 'M',    '-.': 'N',    '---': 'O',
    '.--.': 'P',  '--.-': 'Q',  '.-.': 'R',   '...': 'S',   '-': 'T',
    '..-': 'U',   '...-': 'V',  '.--': 'W',   '-..-': 'X',  '-.--': 'Y',
    '--..': 'Z',
    '-----': '0', '.----': '1', '..---': '2', '...--': '3', '....-': '4',
    '.....': '5', '-....': '6', '--...': '7', '---..': '8', '----.': '9',
    '.-.-.-': '.', '--..--': ',', '..--..': '?', '-..-.': '/',
}

current_symbols = ''   # dots and dashes for the current letter
decoded_text    = ''   # full decoded sentence so far

press_start  = 0
release_time = time.ticks_ms()
btn_was_down = False

WORD_GAP   = 1500  # ms of silence = space between words
LETTER_GAP =  600  # ms of silence = end of letter
DOT_MAX    =  300  # max press duration for a dot
NOISE_MIN  =   50  # ignore presses shorter than this


def draw():
    """Redraw the full display."""
    oled.fill(0)
    oled.text("Morse Decoder", 0, 0)
    oled.hline(0, 10, 128, 1)

    # Current letter being entered
    oled.text("In: " + current_symbols, 0, 16)

    # Decoded text — wrap at 16 chars per line, show last 2 lines
    words = decoded_text[-32:]  # last 32 chars
    oled.text(words[:16], 0, 36)
    oled.text(words[16:32], 0, 50)

    oled.show()


def decode_letter():
    """Look up the current morse sequence and append to decoded text."""
    global current_symbols, decoded_text
    if current_symbols:
        letter = MORSE.get(current_symbols, '?')
        decoded_text += letter
        current_symbols = ''
        # Clear text after 64 chars to avoid overflow
        if len(decoded_text) > 64:
            decoded_text = decoded_text[-32:]


draw()

while True:
    now = time.ticks_ms()
    btn_down = btn.value() == 0
    clear_btn_down = clear_btn.value() == 0

    if clear_btn_down:
        current_symbols = ''
        decoded_text = ''
        draw()
        time.sleep_ms(200) # Simple debounce

    if btn_down and not btn_was_down:
        # Button just pressed — record start
        press_start = now
        btn_was_down = True

    elif not btn_down and btn_was_down:
        # Button just released — classify the press duration
        duration = time.ticks_diff(now, press_start)
        release_time = now
        btn_was_down = False

        if duration >= NOISE_MIN:
            if duration < DOT_MAX:
                current_symbols += '.'
            else:
                current_symbols += '-'
            draw()

    elif not btn_down and not btn_was_down:
        # Button is idle — check gaps
        idle = time.ticks_diff(now, release_time)

        if current_symbols and idle >= LETTER_GAP:
            # Long enough pause: decode the letter
            decode_letter()
            draw()
            release_time = now  # prevent re-triggering immediately

        if idle >= WORD_GAP and decoded_text and not decoded_text.endswith(' '):
            # Even longer pause: word separator
            decoded_text += ' '
            draw()
            release_time = now

    time.sleep_ms(10)
