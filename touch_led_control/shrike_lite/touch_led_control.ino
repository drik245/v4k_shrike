/*
 * Touch LED Control - Shrike Lite (RP2040)
 * ========================================
 * Uses a digital touch sensor (like TTP223) connected to a GPIO pin 
 * to toggle the onboard LED.
 */

#define TOUCH_PIN 14 // Digital input pin
#define LED_PIN 25   // RP2040 standard onboard LED

bool ledState = false;
int lastTouchState = LOW;

void setup() {
  Serial.begin(115200);
  
  pinMode(TOUCH_PIN, INPUT_PULLDOWN);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);
  
  Serial.println("Starting Digital Touch LED Control (Shrike Lite)...");
}

void loop() {
  int isTouched = digitalRead(TOUCH_PIN);
  
  if (isTouched == HIGH && lastTouchState == LOW) {
    // Rising edge
    ledState = !ledState;
    digitalWrite(LED_PIN, ledState ? HIGH : LOW);
    
    Serial.print("Touched! LED is now ");
    Serial.println(ledState ? "ON" : "OFF");
  }
  
  lastTouchState = isTouched;
  delay(50); // Debounce delay
}
