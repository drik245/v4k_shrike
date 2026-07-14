"""
Project 46: Line Follower Robot (Shrike Fi - ESP32-S3)

Classic 2-wheel line follower using IR sensors and an L298N motor driver.
When both sensors see white (reflective), it goes forward.
If the left sensor sees black (line), it turns left to correct.
If the right sensor sees black (line), it turns right to correct.

Wiring:
  IR Left Out  -> ESP_IO14
  IR Right Out -> ESP_IO7
  
  L298N IN1 (Motor A Fwd) -> ESP_IO4
  L298N IN2 (Motor A Rev) -> ESP_IO5
  L298N IN3 (Motor B Fwd) -> ESP_IO6
  L298N IN4 (Motor B Rev) -> ESP_IO15
"""

from machine import Pin
import time

ir_left  = Pin(14, Pin.IN)
ir_right = Pin(7, Pin.IN)

mot_a1 = Pin(4, Pin.OUT)
mot_a2 = Pin(5, Pin.OUT)
mot_b1 = Pin(6, Pin.OUT)
mot_b2 = Pin(15, Pin.OUT)

def set_motors(a1, a2, b1, b2):
    mot_a1.value(a1)
    mot_a2.value(a2)
    mot_b1.value(b1)
    mot_b2.value(b2)

print("Line Follower Ready.")

while True:
    # Most basic IR sensors output 0 when seeing white (reflective) 
    # and 1 when seeing black (non-reflective line).
    left_sees_line = ir_left.value() == 1
    right_sees_line = ir_right.value() == 1
    
    if not left_sees_line and not right_sees_line:
        # Both see white -> go forward
        set_motors(1, 0, 1, 0)
    elif left_sees_line and not right_sees_line:
        # Left sees line -> turn left
        set_motors(0, 0, 1, 0)
    elif right_sees_line and not left_sees_line:
        # Right sees line -> turn right
        set_motors(1, 0, 0, 0)
    else:
        # Both see line -> stop
        set_motors(0, 0, 0, 0)
        
    time.sleep_ms(10)
