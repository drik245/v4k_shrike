/*
 * Touch LED Control - Shrike Fi (ESP32-S3)
 * ========================================
 * Uses the built-in capacitive touch pins on the ESP32-S3 to toggle the onboard LED.
 */

#define TOUCH_PIN 1 // GPIO 1
#define LED_PIN 21  // Onboard LED

// Threshold for capacitive touch detection
// Values drop when touched. Adjust this based on your physical setup.
#define THRESHOLD 30000 

bool ledState = false;
bool lastTouchState = false;

void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);
  
  Serial.println("Starting Capacitive Touch LED Control...");
}

void loop() {
  int touchValue = touchRead(TOUCH_PIN);
  bool isTouched = (touchValue < THRESHOLD);
  
  if (isTouched && !lastTouchState) {
    // Rising edge of touch
    ledState = !ledState;
    digitalWrite(LED_PIN, ledState ? HIGH : LOW);
    
    Serial.print("Touched! Value: ");
    Serial.print(touchValue);
    Serial.print(" - LED is now ");
    Serial.println(ledState ? "ON" : "OFF");
  }
  
  lastTouchState = isTouched;
  delay(50); // Debounce delay
}
