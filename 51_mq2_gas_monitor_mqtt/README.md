# Project 51 — Wireless MQ2 Gas Monitor (MQTT)

Reads analog gas concentration from an MQ-2 sensor via the ESP32-S3's ADC (Shrike Fi board), converts the raw reading to an approximate CO₂ PPM value, and publishes it wirelessly over WiFi using MQTT to a public broker (`broker.hivemq.com`). Includes a custom HTML/JS dashboard for live viewing in any browser.

## Hardware Required

| Component | Connection | Description |
|---|---|---|
| Shrike Fi (ESP32-S3) | — | Main MCU, runs firmware + WiFi |
| MQ-2 Gas Sensor Module | AO → Voltage Divider → ADC Pin | Analog gas concentration output |
| 10kΩ Resistor | Divider top (MQ2 AO → ADC) | Part of voltage divider |
| 15kΩ Resistor | Divider bottom (ADC → GND) | Part of voltage divider |
| 5V / GND | MQ-2 VCC / GND | Sensor heater power |

### Voltage Divider

The MQ-2 module outputs **0–5V** on its analog pin, but the ESP32-S3 ADC is only safe up to **3.3V**. A simple resistive voltage divider steps the signal down:

```
MQ2 AO ──┬── 10kΩ ──┬── ADC Pin (ESP32-S3)
          │          │
          └── 15kΩ ──┘
                     │
                    GND
```

`V_adc = V_mq2 × (15k / (10k + 15k)) = V_mq2 × 0.6`

At max 5V output: `5 × 0.6 = 3.0V` → safe for the ESP32-S3 ADC.

> **Note:** The MQ-2 sensor has a heating element. Let it warm up for 1–2 minutes after power-on before readings stabilize.

## How It Works

```
MQ-2 Sensor → [Voltage Divider] → ESP32-S3 ADC Read (12-bit, 0–4095)
    → Map raw ADC to approximate CO₂ PPM
    → Publish PPM value to MQTT topic `shrikefi/co2` every 2 seconds
    → Dashboard subscribes via WebSocket and displays live data
```

1. **ADC Read:** The ESP32-S3 reads the voltage-divided analog signal from the MQ-2 sensor.
2. **PPM Mapping:** The raw 12-bit ADC value (0–4095) is mapped to an approximate CO₂ PPM range. This is a rough linear mapping — **not lab-calibrated**.
3. **MQTT Publish:** The PPM value is published as a plain number string to topic `shrikefi/co2` on `broker.hivemq.com` (public broker, no auth required) every 2 seconds.
4. **Dashboard:** A browser-based HTML dashboard subscribes to the same topic via MQTT-over-WebSocket (WSS) and shows the live reading.

## Frontend Dashboard

The file `co2_dashboard.html` is a single-file, zero-dependency HTML dashboard:

- **MQTT-over-WebSocket** via [mqtt.js](https://github.com/mqttjs/MQTT.js) CDN — connects to `wss://broker.hivemq.com:8884/mqtt`
- **Live line chart** (last 30 readings) via [Chart.js](https://www.chartjs.org/) CDN
- **Color-coded status:** Green (< 800 PPM), Yellow (800–1500 PPM), Red (> 1500 PPM)
- **Auto-reconnect** on disconnect (3-second retry)
- Dark-mode, mobile-responsive, works on phone browser
- Just open the file directly in any browser — no build step, no server required

## Firmware

The ESP32-S3 firmware for this project was generated using **[SmowCode](https://smowcode.com)** — a flowchart-based visual code generation tool for microcontrollers. The compiled firmware is flashed via USB.

> SmowCode generates Arduino-compatible C++ code from a visual flowchart. If you want to modify the firmware logic, you can either edit the generated `.ino` file directly or redesign the flowchart in SmowCode and re-export.

## MQTT Details

| Parameter | Value |
|---|---|
| Broker | `broker.hivemq.com` (public, free, no auth) |
| Port (ESP32, plain MQTT) | `1883` |
| Port (Dashboard, WSS) | `8884` |
| Topic | `shrikefi/co2` |
| Payload | Plain number string (e.g. `742`) |
| Publish interval | Every 2 seconds |
| QoS | 0 (fire and forget) |

## Setup & Usage

### 1. Flash the ESP32-S3
- Open the firmware `.ino` file in Arduino IDE (or flash via SmowCode)
- Select board: `ESP32S3 Dev Module`
- Update your **WiFi SSID** and **password** in the code
- Upload and open Serial Monitor at `115200` baud to verify connection

### 2. Wire the MQ-2 Sensor
- Connect MQ-2 VCC → 5V, GND → GND
- Connect MQ-2 AO → voltage divider → ESP32-S3 ADC pin
- Wait 1–2 minutes for sensor warm-up

### 3. View Live Data

**Option A — Dashboard (recommended):**
Open `co2_dashboard.html` in any browser. It auto-connects to HiveMQ and shows live PPM values.

**Option B — MQTT Explorer:**
1. Download [MQTT Explorer](https://mqtt-explorer.com/)
2. Connect to `broker.hivemq.com` port `1883`
3. Subscribe to `shrikefi/co2`
4. Watch values stream in every 2 seconds

**Option C — Terminal (mosquitto_sub):**
```bash
mosquitto_sub -h broker.hivemq.com -t shrikefi/co2
```

## Calibration

The MQ-2 is a general-purpose gas sensor (detects LPG, methane, smoke, CO, etc.) — **not a precision CO₂ instrument**. The PPM values shown are approximate analog-mapped estimates.

To tune thresholds for your environment:
1. Power on in clean air, let sensor warm up (2+ minutes)
2. Note the baseline PPM on the dashboard
3. Adjust `THRESHOLD_MODERATE` and `THRESHOLD_DANGER` in `co2_dashboard.html` to match your observations
