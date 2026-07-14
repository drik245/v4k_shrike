"""
Project 44: Smart Dustbin (Shrike Fi - ESP32-S3)

An auto-opening dustbin lid. Uses an HC-SR04 ultrasonic sensor to detect
a hand approaching, and a servo motor to open the lid for 3 seconds.

Wiring:
  Ultrasonic TRIG -> ESP_IO14
  Ultrasonic ECHO -> ESP_IO7
  Servo Signal    -> ESP_IO6

  Ultrasonic VCC -> 5V (or 3.3V if your sensor supports it)
  Servo VCC      -> 5V
  All GNDs       -> GND
"""

from machine import Pin, PWM, time_pulse_us
import time

# Pins
trig = Pin(14, Pin.OUT)
echo = Pin(7, Pin.IN)
servo = PWM(Pin(6), freq=50)

# Constants
SPEED_OF_SOUND = 0.0343 # cm per microsecond
DETECT_DIST_CM = 20.0   # Trigger distance in cm
OPEN_TIME_MS   = 3000   # How long to keep lid open

# Servo angles (in nanoseconds for 50Hz PWM)
# 0 deg = ~500us (500,000ns), 90 deg = ~1500us (1,500,000ns)
SERVO_CLOSED_NS = 500_000
SERVO_OPEN_NS   = 1_500_000

def get_distance():
    # Send 10us pulse
    trig.value(0)
    time.sleep_us(2)
    trig.value(1)
    time.sleep_us(10)
    trig.value(0)
    
    # Measure echo pulse duration (timeout 30000us ~ 500cm)
    duration = time_pulse_us(echo, 1, 30000)
    
    if duration < 0:
        return 999.0 # Out of range or error
        
    # Distance = (time / 2) * speed of sound
    return (duration / 2.0) * SPEED_OF_SOUND

# Initialization: Close the lid
servo.duty_ns(SERVO_CLOSED_NS)
print("Smart Dustbin Active. Waiting for hand...")
time.sleep(1)

while True:
    dist = get_distance()
    
    if dist < DETECT_DIST_CM:
        print(f"Hand detected at {dist:.1f} cm! Opening lid.")
        servo.duty_ns(SERVO_OPEN_NS)
        time.sleep_ms(OPEN_TIME_MS)
        
        print("Closing lid.")
        servo.duty_ns(SERVO_CLOSED_NS)
        
    time.sleep_ms(100) # Small delay between pings to prevent echo overlap
