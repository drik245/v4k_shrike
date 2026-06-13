/*
 * Touch LED Control – Shrike Lite (RP2040)
 * ========================================
 * Board target : Raspberry Pi Pico (Generic RP2040)
 * Core         : arduino-pico (Earle Philhower)
 *
 * Uses a digital touch sensor (like TTP223) connected to a GPIO pin
 * to toggle the onboard LED with single/double tap detection.
 *
 *   Single tap  → LED ON/OFF toggle
 *   Double tap  → LED blink mode ON
 *   Double tap  → LED blink mode OFF (LED turns off)
 *
 * Wiring (Shrike Lite header):
 *   Touch sensor → RP_IO27  (GPIO 27, digital input)
 *   Onboard LED  → RP_IO4   (GPIO 4)
 *
 * No external libraries required.
 */

// ── Hardware Pins (Shrike Lite) ──
#define TOUCH_PIN  27  // RP_IO27
#define LED_PIN     4  // RP_IO4 – onboard LED

bool ledState   = false;
int  buttonState = LOW;
int  lastReading = LOW;

unsigned long lastDebounceTime = 0;
unsigned long debounceDelay    = 50;

// Multi-tap logic
int  clickCount = 0;
unsigned long lastClickTime = 0;
const unsigned long DOUBLE_TAP_WINDOW = 300; // ms

// Blinking logic
bool isBlinking = false;
unsigned long lastBlinkTime = 0;
const unsigned long BLINK_INTERVAL = 250;

void setup() {
  Serial.begin(115200);

  pinMode(TOUCH_PIN, INPUT_PULLDOWN);
  pinMode(LED_PIN,   OUTPUT);
  digitalWrite(LED_PIN, LOW);

  Serial.println("[TOUCH] Starting Digital Touch LED Control (Shrike Lite)...");
}

void loop() {
  int reading = digitalRead(TOUCH_PIN);

  // ── Debounce ──
  if (reading != lastReading) {
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > debounceDelay) {
    if (reading != buttonState) {
      buttonState = reading;

      // Rising edge — touch detected
      if (buttonState == HIGH) {
        clickCount++;
        lastClickTime = millis();
      }
    }
  }
  lastReading = reading;

  // ── Evaluate taps after timeout ──
  if (clickCount > 0 && (millis() - lastClickTime) > DOUBLE_TAP_WINDOW) {
    if (clickCount == 1) {
      isBlinking = false;
      ledState = !ledState;
      digitalWrite(LED_PIN, ledState ? HIGH : LOW);
      Serial.print("[TOUCH] Single Tap → LED ");
      Serial.println(ledState ? "ON" : "OFF");
    } else if (clickCount >= 2) {
      isBlinking = !isBlinking;
      if (!isBlinking) {
        ledState = false;
        digitalWrite(LED_PIN, LOW);
      }
      Serial.print("[TOUCH] Double Tap → Blink ");
      Serial.println(isBlinking ? "ON" : "OFF");
    }
    clickCount = 0;
  }

  // ── Blink handler ──
  if (isBlinking && (millis() - lastBlinkTime >= BLINK_INTERVAL)) {
    lastBlinkTime = millis();
    digitalWrite(LED_PIN, !digitalRead(LED_PIN));
  }
}
