/*
 * Touch LED Control – Shrike Fi (ESP32-S3)
 * ========================================
 * Board target : ESP32-S3 Dev Module (Generic)
 *
 * Uses the built-in capacitive touch pins on the ESP32-S3 to toggle
 * the onboard LED with single/double tap detection.
 *
 *   Single tap  → LED ON/OFF toggle
 *   Double tap  → LED blink mode ON
 *   Double tap  → LED blink mode OFF (LED turns off)
 *
 * Wiring (Shrike Fi header):
 *   Touch input → ESP_IO1  (GPIO 1, Touch IO 1)
 *   Onboard LED → ESP_IO21 (GPIO 21)
 *
 * No external libraries required.
 */

// ── Hardware Pins (Shrike Fi) ──
#define TOUCH_PIN  1   // ESP_IO1 – Touch IO 1
#define LED_PIN    21  // ESP_IO21 – Onboard LED

// Capacitive touch threshold (lower = touched on ESP32-S3)
#define THRESHOLD  30000

bool ledState    = false;
bool buttonState = false;
bool lastReading = false;

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
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  Serial.println("[TOUCH] Starting Capacitive Touch LED Control...");
}

void loop() {
  int touchValue = touchRead(TOUCH_PIN);
  bool reading = (touchValue < THRESHOLD);

  // ── Debounce ──
  if (reading != lastReading) {
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > debounceDelay) {
    if (reading != buttonState) {
      buttonState = reading;

      // Rising edge — finger touched
      if (buttonState == true) {
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
