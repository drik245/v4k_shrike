/*
 * Touch LED Control - Shrike Fi (ESP32-S3)
 * ========================================
 * Uses the built-in capacitive touch pins on the ESP32-S3 to toggle 
 * the onboard LED with Single and Double Tap support.
 */

#define TOUCH_PIN 1 // ESP_IO1 (Touch IO 1)
#define LED_PIN 21  // ESP_IO21 (Onboard LED)

// Threshold for capacitive touch detection
#define THRESHOLD 30000 

bool ledState = false;
bool buttonState = false;
bool lastReading = false;

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
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);
  
  Serial.println("Starting Capacitive Touch LED Control...");
}

void loop() {
  int touchValue = touchRead(TOUCH_PIN);
  bool reading = (touchValue < THRESHOLD);
  
  if (reading != lastReading) {
    lastDebounceTime = millis();
  }
  
  if ((millis() - lastDebounceTime) > debounceDelay) {
    if (reading != buttonState) {
      buttonState = reading;
      
      // Rising edge
      if (buttonState == true) {
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
        // Stop blinking -> turn off LED
        ledState = false;
        digitalWrite(LED_PIN, LOW);
      }
      Serial.print("Double Tap! Blinking is now ");
      Serial.println(isBlinking ? "ON" : "OFF");
    }
    clickCount = 0;
  }

  // Handle blinking
  if (isBlinking) {
    if (millis() - lastBlinkTime >= blinkInterval) {
      lastBlinkTime = millis();
      digitalWrite(LED_PIN, !digitalRead(LED_PIN)); // Toggle LED
    }
  }
}
