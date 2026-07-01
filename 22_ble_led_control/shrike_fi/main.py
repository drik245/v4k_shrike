# Project 22: BLE LED Control - Shrike Fi (ESP32-S3)
# MicroPython - Controls an LED via BLE UART (Nordic UART Service)
# Use nRF Connect app to send: LED_ON, LED_OFF, STATUS

import bluetooth
import time
from machine import Pin
from micropython import const

# BLE UART UUIDs (Nordic UART Service)
_UART_UUID = bluetooth.UUID("6E400001-B5A3-F393-E0A9-E50E24DCCA9E")
_UART_TX   = bluetooth.UUID("6E400003-B5A3-F393-E0A9-E50E24DCCA9E")
_UART_RX   = bluetooth.UUID("6E400002-B5A3-F393-E0A9-E50E24DCCA9E")

_FLAG_READ  = const(0x0002)
_FLAG_WRITE = const(0x0008)
_FLAG_NOTIFY = const(0x0010)

_UART_SERVICE = (
    _UART_UUID,
    (
        (_UART_TX, _FLAG_READ | _FLAG_NOTIFY),
        (_UART_RX, _FLAG_WRITE),
    ),
)

# Events
_IRQ_CENTRAL_CONNECT    = const(1)
_IRQ_CENTRAL_DISCONNECT = const(2)
_IRQ_GATTS_WRITE        = const(3)

# LED on ESP_IO21
led = Pin(21, Pin.OUT)
led.value(0)

class BLEUART:
    def __init__(self, ble, name="ShrikeFi-BLE"):
        self._ble = ble
        self._ble.active(True)
        self._ble.irq(self._irq)
        ((self._tx_handle, self._rx_handle),) = self._ble.gatts_register_services((_UART_SERVICE,))
        self._connections = set()
        self._rx_buffer = bytearray()
        self._name = name
        self._advertise()

    def _irq(self, event, data):
        if event == _IRQ_CENTRAL_CONNECT:
            conn_handle, _, _ = data
            self._connections.add(conn_handle)
            print("Connected!")
            led.value(1)  # solid ON when connected
            self.send("Shrike Fi BLE Ready!\r\n")

        elif event == _IRQ_CENTRAL_DISCONNECT:
            conn_handle, _, _ = data
            self._connections.discard(conn_handle)
            print("Disconnected!")
            self._advertise()

        elif event == _IRQ_GATTS_WRITE:
            conn_handle, value_handle = data
            if value_handle == self._rx_handle:
                msg = self._ble.gatts_read(self._rx_handle).decode().strip()
                print("Received:", msg)
                self._handle_command(msg)

    def _handle_command(self, cmd):
        cmd = cmd.upper()
        if cmd == "LED_ON":
            led.value(1)
            self.send("LED is ON\r\n")
            print("LED turned ON")
        elif cmd == "LED_OFF":
            led.value(0)
            self.send("LED is OFF\r\n")
            print("LED turned OFF")
        elif cmd == "STATUS":
            state = "ON" if led.value() else "OFF"
            self.send("LED is " + state + "\r\n")
            print("Status queried: LED is", state)
        else:
            self.send("Unknown: " + cmd + "\r\nUse: LED_ON, LED_OFF, STATUS\r\n")

    def send(self, data):
        for conn_handle in self._connections:
            self._ble.gatts_notify(conn_handle, self._tx_handle, data.encode())

    def _advertise(self, interval_us=500000):
        name_bytes = self._name.encode()
        # ADV payload: flags + name
        adv_data = bytearray()
        adv_data += bytes([0x02, 0x01, 0x06])  # flags
        adv_data += bytes([len(name_bytes) + 1, 0x09]) + name_bytes  # complete name
        self._ble.gap_advertise(interval_us, adv_data=adv_data)
        print("Advertising as:", self._name)


# main
ble = bluetooth.BLE()
uart = BLEUART(ble, "ShrikeFi-BLE")

print("BLE LED Control running on Shrike Fi")
print("Connect with nRF Connect and send: LED_ON, LED_OFF, STATUS")

# blink LED while waiting for connection
while True:
    if not uart._connections:
        led.value(not led.value())
        time.sleep_ms(500)
    else:
        time.sleep_ms(100)
