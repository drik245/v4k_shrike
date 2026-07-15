"""
Project 41: Automatic Telegrapher Keyer (Shrike Fi - ESP32-S3)

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

from machine import Pin, SPI, PWM
from ssd1306 import SSD1306_SPI
import time

# Hardware SPI2 — ESP32-S3 default hardware pins
spi = SPI(2, baudrate=10_000_000, sck=Pin(36), mosi=Pin(35), miso=Pin(37))
oled = SSD1306_SPI(128, 64, spi, dc=Pin(4), res=Pin(5), cs=Pin(34))

# Input Buttons
dot_btn = Pin(11, Pin.IN, Pin.PULL_UP)
dash_btn = Pin(14, Pin.IN, Pin.PULL_UP)
clear_btn = Pin(15, Pin.IN, Pin.PULL_UP)

# Buzzer
buzzer = PWM(Pin(10))
buzzer.freq(1000) # 1kHz tone
buzzer.duty_u16(0) # Off initially

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
last_input_time = time.ticks_ms()

# --- Configurable Settings ---
DOT_BEEP_MS     = 100
DASH_BEEP_MS    = 300
SYMBOL_SPACE_MS = 100  # Pause after a dot/dash before the next one can start
WORD_GAP        = 1500 # ms of silence = space between words
LETTER_GAP      = 600  # ms of silence = end of letter
# -----------------------------

def draw():
    """Redraw the full display."""
    oled.fill(0)
    oled.text("Auto Keyer", 0, 0)
    oled.hline(0, 10, 128, 1)

    # Current letter being entered
    oled.text("In: " + current_symbols, 0, 16)

    # Decoded text — wrap at 16 chars per line, show last 2 lines
    words = decoded_text[-32:]
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
        if len(decoded_text) > 64:
            decoded_text = decoded_text[-32:]

def beep(duration_ms):
    buzzer.duty_u16(32768) # 50% duty cycle to play tone
    time.sleep_ms(duration_ms)
    buzzer.duty_u16(0)     # Turn off tone

draw()

while True:
    now = time.ticks_ms()
    
    # Read buttons (0 = pressed because of PULL_UP)
    dot_down = (dot_btn.value() == 0)
    dash_down = (dash_btn.value() == 0)
    clear_down = (clear_btn.value() == 0)

    if clear_down:
        current_symbols = ''
        decoded_text = ''
        last_input_time = now
        draw()
        time.sleep_ms(200) # Simple debounce
        continue

    if dot_down:
        # Dot pressed! Auto-repeats if held down.
        beep(DOT_BEEP_MS)
        current_symbols += '.'
        last_input_time = time.ticks_ms()
        draw()
        time.sleep_ms(SYMBOL_SPACE_MS)
    
    elif dash_down:
        # Dash pressed! Auto-repeats if held down.
        beep(DASH_BEEP_MS)
        current_symbols += '-'
        last_input_time = time.ticks_ms()
        draw()
        time.sleep_ms(SYMBOL_SPACE_MS)

    # Check for pauses to decode letters or insert spaces
    if not dot_down and not dash_down:
        idle = time.ticks_diff(now, last_input_time)

        if current_symbols and idle >= LETTER_GAP:
            decode_letter()
            draw()
            last_input_time = now

        if idle >= WORD_GAP and decoded_text and not decoded_text.endswith(' '):
            decoded_text += ' '
            draw()
            last_input_time = now

    time.sleep_ms(10)
