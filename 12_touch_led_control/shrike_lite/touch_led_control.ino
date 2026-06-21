/*
  Touch LED Control - Shrike Lite (RP2040)

  Uses a digital touch sensor (like TTP223) to toggle
  the onboard LED with single/double tap.

  Single tap  - LED on/off toggle
  Double tap  - LED blink mode on
  Double tap  - LED blink mode off

  Wiring:
    Touch sensor - RP_IO27 (GPIO 27)
    Onboard LED  - RP_IO4 (GPIO 4)

  No external libraries needed.
*/

// pins
#define TOUCH_PIN  27
#define LED_PIN     4

bool ledState   = false;
int  buttonState = LOW;
int  lastReading = LOW;

unsigned long lastDebounceTime = 0;
unsigned long debounceDelay    = 50;

// multi-tap logic
int  clickCount = 0;
unsigned long lastClickTime = 0;
const unsigned long DOUBLE_TAP_WINDOW = 300;

// blinking
bool isBlinking = false;
unsigned long lastBlinkTime = 0;
const unsigned long BLINK_INTERVAL = 250;

void setup() {
  Serial.begin(115200);

  pinMode(TOUCH_PIN, INPUT_PULLDOWN);
  pinMode(LED_PIN,   OUTPUT);
  digitalWrite(LED_PIN, LOW);

  Serial.println("touch led control ready (shrike lite)");
}

void loop() {
  int reading = digitalRead(TOUCH_PIN);

  // debounce
  if (reading != lastReading) {
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > debounceDelay) {
    if (reading != buttonState) {
      buttonState = reading;

      if (buttonState == HIGH) {
        clickCount++;
        lastClickTime = millis();
      }
    }
  }
  lastReading = reading;

  // evaluate taps after timeout
  if (clickCount > 0 && (millis() - lastClickTime) > DOUBLE_TAP_WINDOW) {
    if (clickCount == 1) {
      isBlinking = false;
      ledState = !ledState;
      digitalWrite(LED_PIN, ledState ? HIGH : LOW);
      Serial.print("single tap, LED ");
      Serial.println(ledState ? "on" : "off");
    } else if (clickCount >= 2) {
      isBlinking = !isBlinking;
      if (!isBlinking) {
        ledState = false;
        digitalWrite(LED_PIN, LOW);
      }
      Serial.print("double tap, blink ");
      Serial.println(isBlinking ? "on" : "off");
    }
    clickCount = 0;
  }

  // blink handler
  if (isBlinking && (millis() - lastBlinkTime >= BLINK_INTERVAL)) {
    lastBlinkTime = millis();
    digitalWrite(LED_PIN, !digitalRead(LED_PIN));
  }
}
