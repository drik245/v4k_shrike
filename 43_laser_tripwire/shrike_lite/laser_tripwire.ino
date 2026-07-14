/*
  Project 43: Laser Tripwire Alarm (Shrike Lite - RP2040)
  
  A classic laser security system!
  When the laser beam hitting the LDR is broken, the alarm triggers and
  latches ON until the Reset button is pressed.

  Wiring:
    LDR (Light Sensor) -> RP_IO26 (ADC0) and pull-down resistor to GND
    Laser Module       -> RP_IO15
    Active Buzzer (+)  -> RP_IO10
    Reset Button       -> RP_IO14 to GND (INPUT_PULLUP)
*/

#include <Arduino.h>

#define PIN_LDR    26 // RP_IO26
#define PIN_LASER  15 // RP_IO15
#define PIN_BUZZER 10 // RP_IO10
#define PIN_RESET  14 // RP_IO14

enum State { ARMING, ARMED, ALARM };
State currentState = ARMING;

int threshold = 0;

void setup() {
  Serial.begin(115200);
  pinMode(PIN_LASER, OUTPUT);
  pinMode(PIN_BUZZER, OUTPUT);
  pinMode(PIN_RESET, INPUT_PULLUP);

  // Turn on the laser for calibration
  digitalWrite(PIN_LASER, HIGH);
  Serial.println("System Booting. Please ensure laser is hitting the LDR.");
  
  // Quick startup beep
  tone(PIN_BUZZER, 2000, 100);
  delay(150);
  tone(PIN_BUZZER, 2000, 100);
  
  // Wait 3 seconds for the user to align the laser
  delay(3000); 

  // Calibrate threshold
  int currentLight = analogRead(PIN_LDR);
  // Threshold is 75% of the direct laser light value.
  // If it drops below this, the beam is broken!
  threshold = currentLight * 0.75; 

  Serial.print("Calibration complete. Threshold set to: ");
  Serial.println(threshold);
  Serial.println("System ARMED.");
  
  // Armed beep
  tone(PIN_BUZZER, 3000, 500);
  currentState = ARMED;
}

void loop() {
  if (currentState == ARMED) {
    int lightLevel = analogRead(PIN_LDR);
    
    // If light level drops below the threshold, the beam was broken
    if (lightLevel < threshold) {
      Serial.println("ALARM! Beam broken!");
      currentState = ALARM;
    }
    delay(10); // Small delay for stability
  } 
  else if (currentState == ALARM) {
    // Siren effect
    tone(PIN_BUZZER, 1000);
    delay(100);
    tone(PIN_BUZZER, 1500);
    delay(100);
    
    // Check for reset button press to clear the alarm
    if (digitalRead(PIN_RESET) == LOW) {
      noTone(PIN_BUZZER);
      Serial.println("Alarm Reset. System ARMED.");
      delay(500); // Debounce
      currentState = ARMED;
    }
  }
}
