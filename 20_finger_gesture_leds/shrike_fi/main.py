import machine
import sys
import select

# setup 5 LEDs for Shrike Fi (ESP_IO1 to ESP_IO5)
led_pins = [1, 2, 3, 4, 5]
leds = []

for pin_num in led_pins:
    pin = machine.Pin(pin_num, machine.Pin.OUT)
    pin.value(0) # turn off initially
    leds.append(pin)

# setup non-blocking stdin poller
poller = select.poll()
poller.register(sys.stdin, select.POLLIN)

print("Shrike Fi MicroPython Ready! Listening for 'LEDS:x' commands...")

def update_leds(count):
    # clamp count between 0 and 5
    count = max(0, min(5, count))
    for i in range(5):
        if i < count:
            leds[i].value(1) # turn on
        else:
            leds[i].value(0) # turn off

while True:
    # check if data is waiting on stdin (timeout 100ms)
    events = poller.poll(100)
    
    if events:
        # read the incoming line
        line = sys.stdin.readline().strip()
        
        # parse command
        if line.startswith("LEDS:"):
            try:
                count = int(line.split(":")[1])
                update_leds(count)
                # optionally print back for debug, but careful not to confuse the PC script
                print(f"ACK: Set {count} LEDs")
            except ValueError:
                pass
