/*
 * TM1637 Button-Press Counter – Shrike Lite (RP2040)
 * ===================================================
 * Board target : Raspberry Pi Pico (Generic RP2040)
 * Core         : arduino-pico (Earle Philhower)
 *
 * A simple button-based digital counter using a TM1637 4-digit
 * seven-segment display. Press the button to increment, hold to
 * reset. Count range: 0000 → 9999 (wraps around).
 *
 * Wiring (Shrike Lite header):
 *   TM1637 CLK → RP_IO10  (GPIO 10)
 *   TM1637 DIO → RP_IO11  (GPIO 11)
 *   TM1637 VCC → 3.3V
 *   TM1637 GND → GND
 *   Button     → RP_IO28  (GPIO 28, active LOW with internal pull-up)
 *
 * Dependencies:
 *   TM1637Display library (by Avishay Orpaz)
 */

#include <TM1637Display.h>

// ── Hardware Pins (Shrike Lite) ──
#define TM_CLK     10  // RP_IO10
#define TM_DIO     11  // RP_IO11
#define BUTTON_PIN 28  // RP_IO28 (active LOW, internal pull-up)

TM1637Display tm(TM_CLK, TM_DIO);

int count = 0;

// ── Debounce state ──
int  buttonState     = HIGH;
int  lastReading     = HIGH;
unsigned long lastDebounce = 0;
const unsigned long DEBOUNCE_MS = 50;

// ── Long-press detection ──
unsigned long pressStart = 0;
bool          pressed    = false;
const unsigned long LONG_PRESS_MS = 1500; // hold 1.5s to reset

void showCount() {
  tm.showNumberDecEx(count, 0x00, true); // leading zeros, no colon
}

void setup() {
  Serial.begin(115200);

  pinMode(BUTTON_PIN, INPUT_PULLUP);

  tm.setBrightness(5); // 0-7

  // Show "0000" on start
  showCount();
  Serial.println("[CNT] Button Counter ready — press to count, hold to reset");
}

void loop() {
  int reading = digitalRead(BUTTON_PIN);

  // ── Debounce ──
  if (reading != lastReading) {
    lastDebounce = millis();
  }

  if ((millis() - lastDebounce) > DEBOUNCE_MS) {
    if (reading != buttonState) {
      buttonState = reading;

      if (buttonState == LOW) {
        // Button just pressed — start timer
        pressStart = millis();
        pressed = true;
      } else {
        // Button released
        if (pressed) {
          unsigned long held = millis() - pressStart;

          if (held >= LONG_PRESS_MS) {
            // Long press → reset
            count = 0;
            Serial.println("[CNT] RESET → 0000");
          } else {
            // Short press → increment
            count++;
            if (count > 9999) count = 0; // wrap
            Serial.print("[CNT] Count: ");
            Serial.println(count);
          }

          showCount();
          pressed = false;
        }
      }
    }
  }

  // ── Visual feedback: blink display while held long enough ──
  if (pressed && buttonState == LOW) {
    unsigned long held = millis() - pressStart;
    if (held >= LONG_PRESS_MS) {
      if ((millis() / 300) % 2 == 0) {
        tm.showNumberDecEx(0, 0x00, true);
      } else {
        tm.clear();
      }
    }
  }

  lastReading = reading;
}
