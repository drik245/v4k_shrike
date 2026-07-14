from machine import Pin, UART
import time

# Shrike Lite (RP2040) configuration
# UART0 on RP_IO16 (TX) and RP_IO17 (RX)
uart = UART(0, baudrate=9600, tx=Pin(16), rx=Pin(17))

led = Pin(14, Pin.OUT)
led.value(0) # Turn off initially

print("HC-05 Bluetooth LED Control (Shrike Lite) Ready!")
print("Waiting for data...")

while True:
    if uart.any():
        data = uart.read().decode('utf-8').strip()
        print("Received:", data)
        
        if data == '1' or data.lower() == 'on':
            led.value(1)
            uart.write("LED Turned ON\n")
        elif data == '0' or data.lower() == 'off':
            led.value(0)
            uart.write("LED Turned OFF\n")
            
    time.sleep(0.1)
