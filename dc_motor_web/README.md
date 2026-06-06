# ⚡ DC Motor Web Speed Control

Control a DC motor from your phone or laptop browser. The **FPGA generates hardware PWM** for the L298N motor driver while the **ESP32-S3 hosts a Wi-Fi web server** with a real-time slider UI.

> **Shrike Fi only** — requires Wi-Fi (ESP32-S3).

## Architecture

```
Browser (slider UI)
    │ HTTP over Wi-Fi
ESP32-S3 (web server + SPI master)
    │ SPI (duty + direction)
SLG47910 FPGA (PWM generator)
    │ FPGA_IO pins
L298N Motor Driver → DC Motor
```

The MCU programs the FPGA bitstream at boot, then sends speed/direction commands over SPI at runtime. The FPGA autonomously generates a ~24 kHz PWM signal and drives the L298N direction pins — no MCU CPU cycles spent on PWM.

## Board Variant

| Variant | Board | MCU | Sketch |
|---------|-------|-----|--------|
| [shrike_fi](shrike_fi/) | Shrike Fi | ESP32-S3 | `shrike_fi/dc_motor_web.ino` |

## Features

- **FPGA hardware PWM** — jitter-free, ~24 kHz, offloaded from the MCU
- **SPI control interface** — MCU sends 2-byte frames (direction + duty cycle)
- **Wi-Fi Access Point** — connect directly, no router needed
- **Web slider UI** — dark-mode, responsive, works on phone and desktop
- **Bidirectional** — full forward / reverse / stop via L298N H-bridge
- **LED status** — solid = idle, blinking = motor active
- **LittleFS Bitstream Loading** — bitstream is stored in the ESP32's flash filesystem (`/bitstream.bin`) and loaded at boot, meaning no separate FPGA programmer is needed and no hardcoded arrays in the firmware.

## Wiring

### MCU ↔ FPGA (onboard 8-line bus)

The SPI connection between ESP32-S3 and SLG47910 is **onboard** — no external wiring needed. The 8-line bus connects them on the PCB:

| FPGA Pin | Function | ESP32-S3 GPIO | Notes |
|----------|----------|--------------|-------|
| Pin 3 | SPI_SCLK | GPIO 12 | SPI clock |
| Pin 4 | SPI_SS | GPIO 10 | SPI chip select |
| Pin 5 | SPI_MOSI | GPIO 11 | SPI data MCU→FPGA |
| Pin 6 | SPI_MISO | GPIO 13 | SPI data FPGA→MCU |
| EN | Enable | GPIO 9 | Activates FPGA design |
| PWR | Power | GPIO 8 | Power-cycles FPGA |

> **Two phases:** During boot, the 4 SPI lines program the bitstream. At runtime, the same 4 lines carry motor commands. See the [MCU-FPGA communication blog](https://blog.vicharak.in/7-ways-your-mcu-can-talk-to-fpga-on-shrike/) for details.

The MCU uses this bus to:
1. Power-cycle the FPGA (PWR pin)
2. Program the FPGA bitstream at boot (SPI)
3. Pulse EN to activate the design
4. Send motor commands (direction + duty cycle) at runtime (SPI)

### FPGA ↔ L298N (external wiring)

| FPGA Pin | Function | L298N Pin | Wire Color (suggested) |
|----------|----------|-----------|----------------------|
| FPGA_IO8 | PWM output | ENA | Yellow |
| FPGA_IO9 | Direction IN1 | IN1 | Green |
| FPGA_IO10 | Direction IN2 | IN2 | Blue |
| GND (header) | Common ground | GND | Black |

These FPGA pins are on the **bottom dual-row FPGA connector** on the Shrike Fi board.

### L298N ↔ Motor

| L298N Pin | Connection |
|-----------|------------|
| OUT1, OUT2 | DC Motor terminals |
| +12V (or +6V) | External motor power supply |
| GND | Common ground (shared with Shrike) |

> **⚠️ Important:** Connect L298N GND to the Shrike board GND. Use an external power supply for the motor (6–12V), **not** the Shrike's USB 5V.

## FPGA Verilog

The FPGA design is in [`fpga/pwm_motor_ctrl.v`](fpga/pwm_motor_ctrl.v):

| Module | Function |
|--------|----------|
| **SPI Slave** | Receives 16-bit frames (2-bit direction + 8-bit duty) from MCU |
| **PWM Generator** | 8-bit counter with prescaler, ~24 kHz output |
| **Direction Decoder** | Maps direction bits to IN1/IN2 for L298N H-bridge |

### SPI Protocol

The MCU sends 2 bytes per transaction:

| Byte | Bits | Value | Meaning |
|------|------|-------|---------|
| 0 | [1:0] | `0b01` | Forward |
| 0 | [1:0] | `0b10` | Reverse |
| 0 | [1:0] | `0b00` | Stop (coast) |
| 0 | [1:0] | `0b11` | Brake (short) |
| 1 | [7:0] | 0–255 | Duty cycle |

## Dependencies

**MCU firmware:**
- ESP32 board package (Arduino IDE)
- No external libraries — uses built-in `WiFi.h`, `WebServer.h`, `SPI.h`

**FPGA toolchain (one of):**
- [Renesas Go Configure Software Hub](https://www.renesas.com/us/en/software-tool/go-configure-software-hub) (recommended)
- Yosys + nextpnr (open source alternative)

## Setup Guide

### Step 1: Compile the FPGA Bitstream

1. Open **Go Configure Software Hub**
2. Create a new project → select **SLG47910**
3. Import `fpga/pwm_motor_ctrl.v`
4. Assign pins to the FPGA (see FPGA ↔ L298N pin table above)
5. Compile → generate `.hex` bitstream
6. Rename the generated `.hex` file to `bitstream.bin`
7. Place it in the `shrike_fi/data/` folder

### Step 2: Upload Firmware & LittleFS

1. **Board**: `ESP32S3 Dev Module` (ESP32 board package)
2. Open `shrike_fi/dc_motor_web.ino`
3. Upload the sketch to Shrike Fi
4. Run the **Arduino IDE LittleFS upload tool** (or equivalent in PlatformIO) to upload the `data/bitstream.bin` to the ESP32's flash filesystem.

### Step 3: Wire the L298N

Connect FPGA_IO8/9/10 from the bottom connector to the L298N as shown in the wiring table above.

### Step 4: Use It

1. Power the Shrike Fi (USB-C) and the L298N (external supply)
2. On your phone/laptop, connect to Wi-Fi: **`Shrike-Motor`** (password: `vicharak123`)
3. Open **`http://192.168.4.1`** in a browser
4. Drag the slider to control motor speed and direction

## Web UI

The web interface features:
- **Slider** — drag left for reverse, right for forward, center = stop
- **Speed readout** — shows current speed percentage (0–100%)
- **Direction indicator** — FORWARD (green) / REVERSE (red) / STOPPED (gray)
- **Quick buttons** — Full REV / STOP / Full FWD
- Dark theme, mobile-responsive

## Project Structure

```
dc_motor_web/
├── README.md                     ← you are here
├── fpga/
│   └── pwm_motor_ctrl.v          ← Verilog source for FPGA
└── shrike_fi/
    ├── dc_motor_web.ino          ← MCU firmware (Arduino)
    └── data/
        └── bitstream.bin         ← compiled FPGA bitstream to be uploaded via LittleFS
```

## Future: Hardware PID

The FPGA can be extended with a hardware PID controller that:
1. Reads a motor encoder (quadrature decoder in FPGA)
2. Computes PID error loop entirely in hardware
3. Takes only a **speed setpoint** from the MCU over SPI
4. Autonomously maintains the target RPM

This is a natural next step once a motor encoder is wired to the FPGA I/O pins.
