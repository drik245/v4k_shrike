import machine
import time
import sys

BOARD = "shrike_lite"

# gpio pins for each board
PINS_POT = {
    "shrike_lite": {"R": 26, "G": 27, "B": 28}, # ADCs must use 26-28 on RP2040
    "shrike_fi":   {"R": 1,  "G": 2,  "B": 3},
}

PINS_LED = {
    "shrike_lite": {"R": 10, "G": 11, "B": 14},
    "shrike_fi":   {"R": 4,  "G": 5,  "B": 6},
}

COMMON_ANODE = True
ALPHA = 0.1 # Smoothing factor for ADC noise

pot_pins = PINS_POT[BOARD]
led_pins = PINS_LED[BOARD]

# setup the knobs
pot = {}
for ch, gpio in pot_pins.items():
    p = machine.ADC(machine.Pin(gpio))
    if BOARD == "shrike_fi":
        p.atten(machine.ADC.ATTN_11DB)
    pot[ch] = p

# setup the led
led = {}
for ch, gpio in led_pins.items():
    p = machine.PWM(machine.Pin(gpio))
    p.freq(1000)
    led[ch] = p

print("Starting RGB Controller on", BOARD.upper())

# Initialize smoothed values
smooth = {ch: pot[ch].read_u16() for ch in ["R", "G", "B"]}

while True:
    for ch in ["R", "G", "B"]:
        raw = pot[ch].read_u16()
        
        # Apply Exponential Moving Average (EMA) filter
        smooth[ch] = int((ALPHA * raw) + ((1.0 - ALPHA) * smooth[ch]))
        
        # Apply Common Anode inversion if enabled
        duty = smooth[ch]
        if COMMON_ANODE:
            duty = 65535 - duty
            
        led[ch].duty_u16(duty)
    
    time.sleep_ms(20)
