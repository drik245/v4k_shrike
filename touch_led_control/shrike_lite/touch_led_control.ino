/*
 * Touch LED Control - Shrike Lite (RP2040)
 * ========================================
 * Uses a digital touch sensor (like TTP223) connected to a GPIO pin 
 * to toggle the onboard LED with Single and Double Tap support.
 */

#define TOUCH_PIN 27 // Digital input pin
#define LED_PIN 4    // RP_IO4 (Shrike Lite onboard LED)

bool ledState = false;
int buttonState = LOW;
int lastReading = LOW;

unsigned long lastDebounceTime = 0;
unsigned long debounceDelay = 50; // 50ms debounce time

// Multi-tap logic
int clickCount = 0;
unsigned long lastClickTime = 0;
const unsigned long doubleTapTimeout = 300; // ms to wait for a second tap

// Blinking logic
bool isBlinking = false;
unsigned long lastBlinkTime = 0;
const unsigned long blinkInterval = 250; // ms per blink toggle

void setup() {
  Serial.begin(115200);
  
  pinMode(TOUCH_PIN, INPUT_PULLDOWN);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);
  
  Serial.println("Starting Digital Touch LED Control (Shrike Lite)...");
}

void loop() {
  int reading = digitalRead(TOUCH_PIN);
  
  // If the switch changed, due to noise or pressing
  if (reading != lastReading) {
    lastDebounceTime = millis();
  }
  
  if ((millis() - lastDebounceTime) > debounceDelay) {
    // Reading has been stable longer than debounce delay
    if (reading != buttonState) {
      buttonState = reading;
      
      // Rising edge (touch detected)
      if (buttonState == HIGH) {
        clickCount++;
        lastClickTime = millis();
      }
    }
  }
  
  lastReading = reading;

  // Check for tap timeouts to execute single or double tap
  if (clickCount > 0 && (millis() - lastClickTime) > doubleTapTimeout) {
    if (clickCount == 1) {
      // Single tap
      isBlinking = false;
      ledState = !ledState;
      digitalWrite(LED_PIN, ledState ? HIGH : LOW);
      Serial.print("Single Tap! LED is now ");
      Serial.println(ledState ? "ON" : "OFF");
    } else if (clickCount >= 2) {
      // Double tap
      isBlinking = !isBlinking;
      if (!isBlinking) {
        // Double tap again to stop blinking -> turn off LED
        ledState = false;
        digitalWrite(LED_PIN, LOW);
      }
      Serial.print("Double Tap! Blinking is now ");
      Serial.println(isBlinking ? "ON" : "OFF");
    }
    clickCount = 0; // Reset counter after action
  }

  // Handle blinking
  if (isBlinking) {
    if (millis() - lastBlinkTime >= blinkInterval) {
      lastBlinkTime = millis();
      digitalWrite(LED_PIN, !digitalRead(LED_PIN)); // Toggle LED
    }
  }
}
