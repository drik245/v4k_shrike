/*
  TM1637 Button-Press Counter - Shrike Lite (RP2040)

  Press the button to increment a counter on a TM1637 display.
  Hold the button for 1.5s to reset. Wraps at 9999.

  Wiring:
    TM1637 CLK - RP_IO10 (GPIO 10)
    TM1637 DIO - RP_IO11 (GPIO 11)
    Button     - RP_IO28 (GPIO 28, active low, internal pullup)

  Needs: TM1637Display library (by Avishay Orpaz)
*/

#include <TM1637Display.h>

// pins
#define TM_CLK     10
#define TM_DIO     11
#define BUTTON_PIN 28

TM1637Display tm(TM_CLK, TM_DIO);

int count = 0;

// debounce
int  buttonState     = HIGH;
int  lastReading     = HIGH;
unsigned long lastDebounce = 0;
const unsigned long DEBOUNCE_MS = 50;

// long press
unsigned long pressStart = 0;
bool          pressed    = false;
const unsigned long LONG_PRESS_MS = 1500;

void showCount() {
  tm.showNumberDecEx(count, 0x00, true);
}

void setup() {
  Serial.begin(115200);

  pinMode(BUTTON_PIN, INPUT_PULLUP);

  tm.setBrightness(5);
  showCount();
  Serial.println("button counter ready, press to count, hold to reset");
}

void loop() {
  int reading = digitalRead(BUTTON_PIN);

  // debounce
  if (reading != lastReading) {
    lastDebounce = millis();
  }

  if ((millis() - lastDebounce) > DEBOUNCE_MS) {
    if (reading != buttonState) {
      buttonState = reading;

      if (buttonState == LOW) {
        pressStart = millis();
        pressed = true;
      } else {
        if (pressed) {
          unsigned long held = millis() - pressStart;

          if (held >= LONG_PRESS_MS) {
            count = 0;
            Serial.println("reset to 0000");
          } else {
            count++;
            if (count > 9999) count = 0;
            Serial.print("count: ");
            Serial.println(count);
          }

          showCount();
          pressed = false;
        }
      }
    }
  }

  // blink display when held long enough to show reset is ready
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
