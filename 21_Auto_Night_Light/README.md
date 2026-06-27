# 🌙 Auto Night Light

An intelligent night light system that uses an LDR (Light Dependent Resistor) to monitor ambient brightness. When the room is lit, the LED breathes gracefully. When the room is dark, the LED glows steadily.

## Board Variants

| Variant | Board | MCU | Script |
|---------|-------|-----|--------|
| [shrike_fi](shrike_fi/) | Shrike Fi | ESP32-S3 | `shrike_fi/auto_night_light.ino` |
| [shrike_lite](shrike_lite/) | Shrike Lite | RP2040 | `shrike_lite/auto_night_light.ino` |

## Hardware Setup

Connect your LDR and LED components to your board:

### Shrike Fi Wiring
| Component | Pin |
|-----------|-----|
| LDR (Analog) | ESP_IO1 |
| LED (PWM) | ESP_IO2 |

### Shrike Lite Wiring
| Component | Pin |
|-----------|-----|
| LDR (Analog) | RP_IO26 |
| LED (PWM) | RP_IO15 |

### Circuit Diagram

**Voltage Divider:**
```
3.3V --- [LDR] ---●--- [10kΩ] --- GND
                  |
                ADC Pin
```

## Code Analysis & Features

This project utilizes advanced, non-blocking techniques to create a smooth, responsive system:

- **Non-Blocking Breathing Animation:** Instead of using `delay()`, the code uses a finite state machine (`enum BreathState { GLOWING, HOLDING, DIMMING }`) along with `micros()` and `millis()` tracking. This allows the system to continue reading the LDR and responding instantly without freezing during the long breathing cycles.
- **High-Resolution PWM Output:** The code utilizes a 12-bit resolution (`4095` maximum value) for controlling the LED. This ensures incredibly smooth brightness transitions compared to standard 8-bit (255) fading.
  - On the **Shrike Fi**, this is handled via ESP32's `ledcAttach()` and `ledcWrite()` functions.
  - On the **Shrike Lite**, this is handled natively via the RP2040's `analogWriteResolution(12)`.
- **Consistent Analog Reads:** By using `analogReadResolution(8)`, both the ESP32-S3 and RP2040 output LDR values in the 0-255 range, making the threshold logic easier to decide.

## 🛠️ Adjusting the Threshold

The trigger point for switching between the "breathing" and "steady glow" states depends on the ambient lighting of your room.

> [!TIP]
> **Calibrating your LDR:**
> 1. Open the **Serial Monitor** at a baud rate of `115200`.
> 2. Watch the numbers printing on the screen (they range from 0 to 255).
> 3. Cover the LDR with your hand (simulating darkness) and observe the values dropping/rising.
> 4. Shine a light on it (simulating daylight) and observe the values dropping/rising.
> 5. Find the value that best represents "dusk" in your environment, and change the `const int threshold = 230;` variable in the code to match your desired trigger point!
